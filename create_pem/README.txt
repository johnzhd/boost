ca.key CA˽Կ��

openssl genrsa -des3 -out ca.key 2048

�������ܺ��CA˽Կ����һ�㲻��Ҫ��һ����

openssl rsa -in ca.key -out ca_decrypted.key

ca.crt CA��֤�飨��Կ����

openssl req -new -x509 -days 7305 -key ca.key -out ca.crt

������server˽Կ��

openssl genrsa -des3 -out server.key 2048

�������ܺ�ķ�����server˽Կ��Win+Apache��Ҫ���ܺ��server˽Կ����

openssl rsa -in server.key -out server.key2

�ӷ�����server˽Կ����ǩ������CSR�ļ���

openssl req -new -key server.key -out server.csr

###ע������Ƿ�����֤�飬commonNameʱ������������*.creke.net��֧��*ͨ���������֮������ݿ���ɲ��������ɸ���֤�飬����������ǩ�������ʼ���commonName����������emailAddress�ǵ����ʼ���ַ###

###he stateOrProvinceName field needed to be the same in the
CA certificate (root) and the request (server)###

ʹ��Root CAΪuserǩ��

ʹ�õ����ļ�:server.csr, ca.crt,ca.key

openssl ca -in server.csr -out server.crt -cert ca.crt -keyfile ca.key

���Rootû�����룬����������������Ρ�y��������server.crt�ɹ���

���������ȷ������openssl�����Ŀ¼���Ƿ��������ļ���:
��demoCA��һ����

�C newcerts��������

�C private��������

��demoCA�ļ������������ļ�:

demoCA �� index.txt

�C serial

index.txtΪ�հ��ļ�,serial�ļ������������磺123.

#############################################

�������������ɣ�����Root CA crt��ʽת��Ϊder��ʽ���

openssl x509 -in ca.crt -outform DER -out ca.der

der��ʽ�������ƶ��豸�����֤��