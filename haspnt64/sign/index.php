<?php
 $GLOBALS['brand'] = 'leecher';
 $GLOBALS['url']   = 'http://localhost';
 $GLOBALS['cert']  = './ts/';
 $GLOBALS['temp']  = sys_get_temp_dir();

/*
 Requirements
 ============
  PHP
  OpenSSL
  One or more certs with the "Time Stamping" extended key usage flag

 Directory Structure
 ===================
  ./ts/stamp.srl

  ./ts/md5/stamp.cfg
  ./ts/md5/stamp.crt
  ./ts/md5/stamp.key
  ./ts/md5/stamp.pwd
  ./ts/md5/stamp-ca-chain.pem

  ./ts/sha1/stamp.cfg
  ./ts/sha1/stamp.crt
  ./ts/sha1/stamp.key
  ./ts/sha1/stamp.pwd
  ./ts/sha1/stamp-ca-chain.pem

  ./ts/sha256/stamp.cfg
  ./ts/sha256/stamp.crt
  ./ts/sha256/stamp.key
  ./ts/sha256/stamp.pwd
  ./ts/sha256/stamp-ca-chain.pem

  ./ts/sha384/stamp.cfg
  ./ts/sha384/stamp.crt
  ./ts/sha384/stamp.key
  ./ts/sha384/stamp.pwd
  ./ts/sha384/stamp-ca-chain.pem

  ./ts/sha512/stamp.cfg
  ./ts/sha512/stamp.crt
  ./ts/sha512/stamp.key
  ./ts/sha512/stamp.pwd
  ./ts/sha512/stamp-ca-chain.pem


 File Formats
 ============
  .srl: Serial Number for Timestamp
    A text file containing a random hexidecimal number. This file must be writable. Every timestamp request will increment this value by 1.

  .cfg: OpenSSL Configuration
    Directives for OpenSSL. Please refer to this SHA-1 example:

    oid_section = new_oids
    [ new_oids ]
    tsa_priv_policy = 2.16.840.1.119000.7    #random constant OID to identify timestamp by your service
    [ tsa ]
    default_tsa = tsa_stamp
    [ tsa_stamp ]
    dir               = ./ts                 #same path as 'cert' global value
    serial            = $dir/stamp.srl
    signer_cert       = $dir/sha1/stamp.crt
    certs             = $dir/sha1/stamp-ca-chain.pem
    signer_key        = $dir/sha1/stamp.key
    default_policy    = tsa_priv_policy
    signer_digest     = sha1
    digests           = md5, sha1, sha256, sha384, sha512
    accuracy          = secs:5               #maximum allowed time offset between server and client
    ordering          = yes
    tsa_name          = yes
    ess_cert_id_chain = yes
    ess_cert_id_alg   = sha1
    crypto_device     = builtin
    [ tsa_ext ]
    authorityKeyIdentifier = keyid:always
    basicConstraints       = critical,CA:false
    extendedKeyUsage       = critical,timeStamping
    keyUsage               = critical,nonRepudiation
    subjectKeyIdentifier   = hash

 .crt: PEM Certificate
   This should be a child certificate of a parent Timestamping CA. It should have the Time Stamping extended key usage flag.

 .key: PEM Private Key (Encrypted)
   This should be the private key of the above .crt file. It should be encrypted with a password (see below).

 .pwd: Private Key Password
   This should be a text file containing the password for the above .key file.

 .pem: Certificate Chain.
   This should be a file containing the parent Timestamping CA and Root CA, chained together with simple concatenation.

 */
 function showPage()
 {
  header('Content-Type: text/plain');
  echo "Welcome to the {$GLOBALS['brand']} Timestamping Service\n";
  echo "\n";
  echo "This service can be used for RFC 3161 and Microsoft Authenticode Timestamping.\n";
  echo "There are two popular timestamping protocols, which are both supported by this service:\n";
  echo "\n";
  echo "RFC 3161\n";
  echo "========\n";
  echo " RFC 3161 <http://www.ietf.org/rfc/rfc3161.txt> timestamping is used by SignTool (using the \"/tr\" parameter) and other applications (such as jarsigner).";
  echo " This timestamping server automatically selects the appropriate signature algorithm with which to sign each timestamp, based on the digest algorithm you specify (e.g., via SignTool's \"/td\" parameter).";
  echo " Alternatively, you may override the automatic selection process by passing a GET parameter ('?td=', '?md=', and '?alg=' are supported):\n";
  echo " - Auto:    {$GLOBALS['url']}/\n";
  echo " - MD5:     {$GLOBALS['url']}/?td=md5\n";
  echo " - SHA-1:   {$GLOBALS['url']}/?td=sha1\n";
  echo " - SHA-256: {$GLOBALS['url']}/?td=sha256\n";
  echo " - SHA-384: {$GLOBALS['url']}/?td=sha384\n";
  echo " - SHA-512: {$GLOBALS['url']}/?td=sha512\n";
  echo "\n";
  echo "Authenticode\n";
  echo "============\n";
  echo " Authenticode <https://msdn.microsoft.com/bb931395.aspx> timestamping is used by older versions of SignTool (using the \"/t\" parameter) and SignCode.";
  echo " Due to this protocol's design, it is not possible for the timestamping server to automatically select the appropriate signature algorithm.";
  echo " The default signature algorithm is SHA-1, but other choices are available via GET parameter ('?td=', '?md=', and '?alg=' are supported):\n";
  echo " - MD5:     {$GLOBALS['url']}/?md=md5\n";
  echo " - SHA-1:   {$GLOBALS['url']}/?md=sha1\n";
  echo " - SHA-256: {$GLOBALS['url']}/?md=sha256\n";
  echo " - SHA-384: {$GLOBALS['url']}/?md=sha384\n";
  echo " - SHA-512: {$GLOBALS['url']}/?md=sha512\n";
 }
 function showError($err)
 {
  header('HTTP/1.1 400 Bad Request');
  header('Content-Type: text/plain');
  echo "Welcome to the {$GLOBALS['brand']} Timestamping Service\n";
  echo "\n";
  echo "Error: $err\n";
 }
 if (!function_exists('random_int')) { function random_int($min,$max) { return rand($min,$max); } }
 function UUIDv4()
 {
  return sprintf('%04x%04x-%04x-%04x-%04x-%04x%04x%04x',
   random_int(0, 0xffff), random_int(0, 0xffff),
   random_int(0, 0xffff),
   random_int(0, 0x0fff) | 0x4000,
   random_int(0, 0x3fff) | 0x8000,
   random_int(0, 0xffff), random_int(0, 0xffff), random_int(0, 0xffff)
  );
 }

 function getMD($get, $default = '')
 {
  $test = '';
  if (array_key_exists('md', $get))
   $test = strtolower($get['md']);
  elseif (array_key_exists('td', $get))
   $test = strtolower($get['td']);
  elseif (array_key_exists('alg', $get))
   $test = strtolower($get['alg']);
  switch ($test)
  {
   case 'md5': return 'md5';
   case 'sha1': return 'sha1';
   case 'sha256': return 'sha256';
   case 'sha384': return 'sha384';
   case 'sha512': return 'sha512';
  }
  return $default;
 }

 function readByte($data, &$idx)
 {
  $val = ord(substr($data, $idx, 1));
  $idx++;
  return $val;
 }
 function readLen($data, &$idx)
 {
  $val = ord(substr($data, $idx, 1));
  $idx++;
  if ($val < 0x80)
   return $val;
  $ct = $val - 0x80;
  $dt = hexdec(bin2hex(substr($data, $idx, $ct)));
  $idx+= $ct;
  return $dt;
 }
 function readChunk($data, &$idx)
 {
  $ret = array();
  $ret['id']   = readByte($data, $idx);
  $ret['size'] = readLen($data, $idx);
  $ret['data'] = substr($data, $idx, $ret['size']);
  $idx+= $ret['size'];
  return $ret;
 }
 function makeArray($data)
 {
  $a = array();
  $idx = 0;
  do
  {
   $d = readChunk($data, $idx);
   switch ($d['id'])
   {
    case 0x30:
     $d['child'] = makeArray($d['data']);
     break;
    case 0xA0:
     $d['child'] = makeArray($d['data']);
     break;
   }
   $a[] = $d;
  } while ($idx < strlen($data));
  return $a;
 }

 function parseASN($post)
 {
  $req = makeArray($post);
  if (count($req) < 1)
   return false;
  for ($i = 0; $i < count($req); $i++)
  {
   $seq1 = $req[$i];
   if ($seq1['id'] !== 0x30)
    continue;
   if (!array_key_exists('child', $seq1))
    continue;
   for ($j = 0; $j < count($seq1['child']); $j++)
   {
    $seq2 = $seq1['child'][$j];
    if ($seq2['id'] !== 0x30)
     continue;
    if (!array_key_exists('child', $seq2))
     continue;
    for ($k = 0; $k < count($seq2['child']); $k++)
    {
     $seq3 = $seq2['child'][$k];
     if ($seq3['id'] !== 0x30)
      continue;
     if (!array_key_exists('child', $seq3))
      continue;
     for ($l = 0; $l < count($seq3['child']); $l++)
     {
      $oid = $seq3['child'][$l];
      if ($oid['id'] !== 0x06)
       continue;
      switch (bin2hex($oid['data']))
      {
       case '2a864886f70d0205': return 'md5';
       case '2b0e03021a': return 'sha1';
       case '608648016503040201': return 'sha256';
       case '608648016503040202': return 'sha384';
       case '608648016503040203': return 'sha512';
      }
     }
    }
   }
  }
  return false;
 }

 function parseAuthenticodeASN($post)
 {
  $req = makeArray($post);
  if (count($req) < 1)
   return false;
  for ($i = 0; $i < count($req); $i++)
  {
   $seq1 = $req[$i];
   if ($seq1['id'] !== 0x30)
    continue;
   if (!array_key_exists('child', $seq1))
    continue;
   $foundOID1 = false;
   for ($j = 0; $j < count($seq1['child']); $j++)
   {
    $oid = $seq1['child'][$j];
    if ($oid['id'] !== 0x06)
     continue;
    if (bin2hex($oid['data']) !== '2b060104018237030201') //timestampRequest (Microsoft code signing)
     continue;
    $foundOID1 = $j;
    break;
   }
   if ($foundOID1 === false)
    continue;

   for ($j = 0; $j < count($seq1['child']); $j++)
   {
    $seq2 = $seq1['child'][$j];
    if ($seq2['id'] !== 0x30)
     continue;
    if (!array_key_exists('child', $seq2))
     continue;
    $foundOID2 = false;
    for ($k = 0; $k < count($seq2['child']); $k++)
    {
     $oid = $seq2['child'][$k];
     if ($oid['id'] !== 0x06)
      continue;
     if (bin2hex($oid['data']) !== '2a864886f70d010701') //data (PKCS #7)
      continue;
     $foundOID2 = $k;
     break;
    }
    if ($foundOID2 === false)
     continue;
    for ($k = 0; $k < count($seq2['child']); $k++)
    {
     $arr = $seq2['child'][$k];
     if ($arr['id'] !== 0xA0)
      continue;
     if (!array_key_exists('child', $arr))
      continue;
     for ($l = 0; $l < count($arr['child']); $l++)
     {
      $str = $arr['child'][$l];
      if ($str['id'] !== 0x04)
       continue;
      return $str['data'];
     }
    }
   }
  }
  return false;
 }

 function rfc3161($post, $alg)
 {
  if ($alg === '')
  {
   $asnData = parseASN($post);
   if ($asnData !== false)
    $alg = $asnData;
  }
  if ($alg === '')
   $alg = 'sha256';
  $tsCfg   = $GLOBALS['cert'].$alg.'/stamp.cfg';
  $tsKey   = $GLOBALS['cert'].$alg.'/stamp.key';
  $tsPass  = $GLOBALS['cert'].$alg.'/stamp.pwd';
  if (!file_exists($tsKey))
  {
   showError('The digest algorithm you selected is unavailable at this time.');
   return false;
  }
  do
  {
   $reqID   = UUIDv4();
   $tsQuery = $GLOBALS['temp'].$reqID.'.tsq';
   $tsOut   = $GLOBALS['temp'].$reqID.'.tsr';
  } while (file_exists($tsQuery) || file_exists($tsOut));
  file_put_contents($tsQuery, $post);

  $cmd = 'openssl ts -reply'.
         " -queryfile $tsQuery".
         " -out $tsOut".
         " -config $tsCfg".
         " -passin file:$tsPass";
  exec($cmd);
  if (!file_exists($tsOut))
  {
   unlink($tsQuery);
   showError('The request could not be signed.');
   return false;
  }
  $data = file_get_contents($tsOut);
  unlink($tsQuery);
  unlink($tsOut);
  return $data;
 }

 function authenticode($post, $md)
 {
  if (bin2hex(substr($post, -3)) === '0d0a00')
   $post = substr($post, 0, -3);
  $post = base64_decode($post);
  $p7data = parseAuthenticodeASN($post);
  if ($p7data === false)
  {
   showError('The request was not valid.');
   return false;
  }
  if ($md === '')
   $md = 'sha1';
  $tsCert  = $GLOBALS['cert'].$md.'/stamp.crt';
  $tsKey   = $GLOBALS['cert'].$md.'/stamp.key';
  $tsPass  = $GLOBALS['cert'].$md.'/stamp.pwd';
  if (!file_exists($tsKey))
  {
   showError('The digest algorithm you selected is unavailable at this time.');
   return false;
  }
  do
  {
   $reqID   = UUIDv4();
   $tsQuery = $GLOBALS['temp'].$reqID.'.tsq';
   $tsOut   = $GLOBALS['temp'].$reqID.'.tsr';
  } while (file_exists($tsQuery) || file_exists($tsOut));
  if (file_exists($tsQuery))
   unlink($tsQuery);
  file_put_contents($tsQuery, $p7data);
  $cmd = 'openssl cms -sign'.
         " -md $md".
         " -in $tsQuery".
         " -out $tsOut".
         " -signer $tsCert".
         " -inkey $tsKey".
         " -passin file:$tsPass".
         " -nosmimecap".
         " -nodetach".
         " -binary".
         " -outform pem";
  exec($cmd);
  if (!file_exists($tsOut))
  {
   unlink($tsQuery);
   showError('The request could not be signed.');
   return false;
  }
  $ret = file_get_contents($tsOut);
  if (strpos($ret, "-----BEGIN CMS-----\n") === false)
  {
   unlink($tsQuery);
   unlink($tsOut);
   showError('The request could not be signed.');
   return false;
  }
  $ret = substr($ret, strpos($ret, "-----BEGIN CMS-----\n") + 20);
  if (strpos($ret, "\n-----END CMS-----") === false)
  {
   unlink($tsQuery);
   unlink($tsOut);
   showError('The request could not be signed.');
   return false;
  }
  $ret = substr($ret, 0, strrpos($ret, "\n-----END CMS-----"));
  unlink($tsQuery);
  unlink($tsOut);
  return $ret;
 }

 $hList = apache_request_headers();
 if (!array_key_exists('Content-Type', $hList))
 {
  showPage();
  return;
 }
 if ($hList['Content-Type'] === 'application/timestamp-query')
 {
  $md = getMD($_GET);
  $reqData = file_get_contents('php://input');
  $retData = rfc3161($reqData, $md);
  if ($retData !== false)
  {
   header('Content-Type: application/timestamp-reply');
   echo $retData;
  }
  return;
 }
 if ($hList['Content-Type'] === 'application/octet-stream')
 {
  $md = getMD($_GET, 'sha1');
  $reqData = file_get_contents('php://input');
  $retData = authenticode($reqData, $md);
  if ($retData !== false)
  {
   header('Content-Type: application/timestamp-reply');
   echo $retData;
  }
  return;
 }
 showError('The Content-Type header must be "application/timestamp-query" or "application/octet-stream".');
?>
