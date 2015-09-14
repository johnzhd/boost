REM create private key
openssl genrsa -des3 -out private.key 2048

REM create csr
openssl req -new -key private.key -out public.csr

REM create crt
openssl x509 -req -days 3650 -in public.csr -signkey private.key -out work.crt

REM create work private key
openssl rsa -in private.key -out work.key

openssl dhparam -out dh512.pem 512






