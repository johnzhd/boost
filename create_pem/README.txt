ca.key CA私钥：

openssl genrsa -des3 -out ca.key 2048

制作解密后的CA私钥，但一般不需要这一步：

openssl rsa -in ca.key -out ca_decrypted.key

ca.crt CA根证书（公钥）：

openssl req -new -x509 -days 7305 -key ca.key -out ca.crt

服务器server私钥：

openssl genrsa -des3 -out server.key 2048

制作解密后的服务器server私钥（Win+Apache需要解密后的server私钥）：

openssl rsa -in server.key -out server.key2

从服务器server私钥生成签名请求CSR文件：

openssl req -new -key server.key -out server.csr

###注意如果是服务器证书，commonName时输入域名，如*.creke.net，支持*通配符，邮箱之后的内容可填可不填，如果生成个人证书，如用来加密签名电子邮件，commonName输入姓名，emailAddress是电子邮件地址###

###he stateOrProvinceName field needed to be the same in the
CA certificate (root) and the request (server)###

使用Root CA为user签名

使用到的文件:server.csr, ca.crt,ca.key

openssl ca -in server.csr -out server.crt -cert ca.crt -keyfile ca.key

如果Root没有密码，以上命令后输入两次”y”，生成server.crt成功。

如果报错，请确定运行openssl命令的目录下是否有如下文件夹:
—demoCA（一级）

– newcerts（二级）

– private（二级）

及demoCA文件夹下有如下文件:

demoCA — index.txt

– serial

index.txt为空白文件,serial文件中输入数字如：123.

#############################################

三个步骤操作完成，附上Root CA crt格式转换为der格式命令：

openssl x509 -in ca.crt -outform DER -out ca.der

der格式适用于移动设备导入根证书