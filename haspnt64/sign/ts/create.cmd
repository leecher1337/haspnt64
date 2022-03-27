set path=%PATH%;..\openssl
openssl genrsa -out stamp-ca.key 4096 -config openssl.cnf
openssl req -new -x509 -days 1826 -key stamp-ca.key -out stamp-ca.crt -config openssl.cnf
openssl genrsa -des3 -out stamp.key 4096 -config openssl.cnf
openssl req -new -key stamp.key -out stamp.csr -config openssl.cnf
openssl x509 -req -days 730 -in stamp.csr -CA stamp-ca.crt -CAkey stamp-ca.key -set_serial 01 -out stamp.crt -extfile extKey.cnf
openssl pkcs12 -export -out stamp.p12 -inkey stamp.key -in stamp.crt -chain -CAfile stamp-ca.crt
copy stamp-ca.crt + stamp-ca.key stamp-ca-chain.pem
copy /Y stamp.crt sha1\
copy /Y stamp.key sha1\
copy /Y stamp-ca-chain.pem sha1\
