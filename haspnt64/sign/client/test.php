<?php
require_once("TrustedTimestamps.php");

$my_hash = sha1("Some Data for testing");

$requestfile_path = TrustedTimestamps::createRequestfile($my_hash);

$response = TrustedTimestamps::signRequestfile($requestfile_path, "http://localhost");
print_r($response);
/*
Array
(
    [response_string] => Shitload of text (base64-encoded Timestamp-Response of the TSA)
    [response_time] => 1299098823
)
*/

echo TrustedTimestamps::getTimestampFromAnswer($response['response_string']); //1299098823

$tsa_cert_chain_file = "ts/sha1/stamp-ca-chain.pem"; //from https://pki.pca.dfn.de/global-services-ca/pub/cacert/chain.txt

$validate = TrustedTimestamps::validate($my_hash, $response['response_string'], $response['response_time'], $tsa_cert_chain_file); 
var_dump($validate); //bool(true)

//now with an incorrect hash. Same goes for a manipulated response string or response time
$validate = TrustedTimestamps::validate(sha1("im not the right hash"), $response['response_string'], $response['response_time'], $tsa_cert_chain_file);
var_dump($validate); //bool(false)
?>
