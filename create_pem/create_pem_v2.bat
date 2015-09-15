SET openssl_path=C:\OpenSSL-Win32\bin
SET path=%path%;%openssl_path%
SET OPENSSL_CONF=%openssl_path%\openssl.cfg

REM 1 ca private key
openssl genrsa -des3 -out ca.key 2048


REM 2 ca decrypted private key
openssl rsa -in ca.key -out ca_decrypted.key

REM 3 ca public key
openssl req -new -x509 -days 7305 -key ca.key -out ca.crt

REM 4 server private key
openssl genrsa -des3 -out server.key 2048

REM 5 server decrypted private key
openssl rsa -in server.key -out server.key2

REM 6 server requre file
openssl req -new -key server.key -out server.csr

REM 7 server public key with ca 
openssl ca -in server.csr -out server.crt -cert ca.crt -keyfile ca.key

REM 8 make ca public key for mobile machine
openssl x509 -in ca.crt -outform DER -out ca.der