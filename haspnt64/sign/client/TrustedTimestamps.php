<?php
/**
 * TrustedTimestamps.php - Creates Timestamp Requestfiles, processes the request at a Timestamp Authority (TSA) after RFC 3161
 *
 * Released under the MIT license (opensource.org/licenses/MIT) Copyright (c) 2015 David Müller
 *
 * bases on OpenSSL and RFC 3161: http://www.ietf.org/rfc/rfc3161.txt
 *
 * WARNING: 
 *     needs openssl ts, which is availible in OpenSSL versions >= 0.99
 *     This is currently (2011-03-02) not the case in Debian
 *     (see http://stackoverflow.com/questions/5043393/openssl-ts-command-not-working-trusted-timestamps)
 *     -> Possibility: Debian Experimentals -> http://wiki.debian.org/DebianExperimental
 * 
 * For OpenSSL on Windows, see
 *     http://www.slproweb.com/products/Win32OpenSSL.html
 *     http://www.switch.ch/aai/support/howto/openssl-windows.html
 * 
 * @version 0.3
 * @author David Müller
 * @package trustedtimestamps
*/

class TrustedTimestamps
{
    /**
     * Creates a Timestamp Requestfile from a hash
     *
     * @param string $hash: The hashed data (sha1)
     * @return string: path of the created timestamp-requestfile
     */
    public static function createRequestfile ($hash)
    {
        if (strlen($hash) !== 40)
            throw new Exception("Invalid Hash.");
            
        $outfilepath = self::createTempFile();
        $cmd = "openssl ts -query -digest ".escapeshellarg($hash)." -cert -out ".escapeshellarg($outfilepath);

        $retarray = array();
        exec($cmd." 2>&1", $retarray, $retcode);
        
        if ($retcode !== 0)
        {
            echo "$cmd\n";
            throw new Exception("OpenSSL does not seem to be installed: ".implode(", ", $retarray));
        }
        
        if (stripos($retarray[0], "openssl:Error") !== false)
            throw new Exception("There was an error with OpenSSL. Is version >= 0.99 installed?: ".implode(", ", $retarray));

        return $outfilepath;
    }

    /**
     * Signs a timestamp requestfile at a TSA using CURL
     *
     * @param string $requestfile_path: The path to the Timestamp Requestfile as created by createRequestfile
     * @param string $tsa_url: URL of a TSA such as http://zeitstempel.dfn.de
     * @return array of response_string with the unix-timetamp of the timestamp response and the base64-encoded response_string
     */
    public static function signRequestfile ($requestfile_path, $tsa_url)
    {
        if (!file_exists($requestfile_path))
            throw new Exception("The Requestfile was not found");

        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $tsa_url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_TIMEOUT, 10);
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_BINARYTRANSFER, 1);
        curl_setopt($ch, CURLOPT_POSTFIELDS, file_get_contents($requestfile_path));
        curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/timestamp-query'));
        curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)"); 
        $binary_response_string = curl_exec($ch);
        $status = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        curl_close($ch);
        
        if ($status != 200 || !strlen($binary_response_string))
            throw new Exception("The request failed (url=$tsa_url, status=$status, resp=$binary_response_string)");
        
        $base64_response_string = base64_encode($binary_response_string);
        
        $response_time = self::getTimestampFromAnswer ($base64_response_string);
        
        return array("response_string" => $base64_response_string,
                     "response_time" => $response_time);
    }

    /**
     * Extracts the unix timestamp from the base64-encoded response string as returned by signRequestfile
     *
     * @param string $base64_response_string: Response string as returned by signRequestfile
     * @return int: unix timestamp
     */
    public static function getTimestampFromAnswer ($base64_response_string)
    {
        $binary_response_string = base64_decode($base64_response_string);

        $responsefile = self::createTempFile($binary_response_string);

        $cmd = "openssl ts -reply -in ".escapeshellarg($responsefile)." -text";
        
        $retarray = array();
        exec($cmd." 2>&1", $retarray, $retcode);
        
        if ($retcode !== 0)
            throw new Exception("The reply failed: ".implode(", ", $retarray));
        
        $matches = array();
        $response_time = 0;

        /*
         * Format of answer:
         * 
         * Foobar: some stuff
         * Time stamp: 21.08.2010 blabla GMT
         * Somestuff: Yayayayaya
         */
        foreach ($retarray as $retline)
        {
            if (preg_match("~^Time\sstamp\:\s(.*)~", $retline, $matches))
            {
                $response_time = strtotime($matches[1]);
                break;        
            }
        }

        if (!$response_time)
            throw new Exception("The Timestamp was not found");    
            
        return $response_time;
    }

    /**
     *
     * @param string $hash: sha1 hash of the data which should be checked
     * @param string $base64_response_string: The response string as returned by signRequestfile
     * @param int $response_time: The response time, which should be checked
     * @param string $tsa_cert_file: The path to the TSAs certificate chain (e.g. https://pki.pca.dfn.de/global-services-ca/pub/cacert/chain.txt)
     * @return <type>
     */
    public static function validate ($hash, $base64_response_string, $response_time, $tsa_cert_file)
    {
        if (strlen($hash) !== 40)
            throw new Exception("Invalid Hash");
        
        $binary_response_string = base64_decode($base64_response_string);
        
        if (!strlen($binary_response_string))
            throw new Exception("There was no response-string");    
            
        if (!intval($response_time))
            throw new Exception("There is no valid response-time given");
        
        if (!file_exists($tsa_cert_file))
            throw new Exception("The TSA-Certificate could not be found");
        
        $responsefile = self::createTempFile($binary_response_string);

        $cmd = "openssl ts -verify -digest ".escapeshellarg($hash)." -in ".escapeshellarg($responsefile)." -CAfile ".escapeshellarg($tsa_cert_file);
        
        $retarray = array();
        exec($cmd." 2>&1", $retarray, $retcode);
        
        /*
         * just 2 "normal" cases: 
         *     1) Everything okay -> retcode 0 + retarray[0] == "Verification: OK"
         *  2) Hash is wrong -> retcode 1 + strpos(retarray[somewhere], "message imprint mismatch") !== false
         * 
         * every other case (Certificate not found / invalid / openssl is not installed / ts command not known)
         * are being handled the same way -> retcode 1 + any retarray NOT containing "message imprint mismatch"
         */
        
        if ($retcode === 0 && strtolower(trim($retarray[0])) == "verification: ok")
        {
            if (self::getTimestampFromAnswer ($base64_response_string) != $response_time)
                throw new Exception("The responsetime of the request was changed");
            
            return true;
        }

        foreach ($retarray as $retline)
        {
            if (stripos($retline, "message imprint mismatch") !== false)
                return false;
        }

        throw new Exception("Systemcommand failed: ".implode(", ", $retarray));
    }

    /**
     * Create a tempfile in the systems temp path
     *
     * @param string $str: Content which should be written to the newly created tempfile
     * @return string: filepath of the created tempfile
     */
    public static function createTempFile ($str = "")
    {
        $tempfilename = tempnam(sys_get_temp_dir(), rand());

        if (!file_exists($tempfilename))
            throw new Exception("Tempfile could not be created");
            
        if (!empty($str) && !file_put_contents($tempfilename, $str))
            throw new Exception("Could not write to tempfile");

        return $tempfilename;
    }
}
?>
