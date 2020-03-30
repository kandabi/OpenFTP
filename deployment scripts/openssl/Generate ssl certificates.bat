@echo off
set opensslDirectory=E:/Program Files/OpenSSL/bin
set /p password=<debug-password.txt && set days=3650 && set drive=%opensslDirectory:~0,2% && set finalOutputDirectory=%CD%/output 

if defined password (ECHO password IS defined)  else (echo password is NOT defined, terminating early && cmd /k && EXIT )  
%drive% && cd %opensslDirectory%


openssl genrsa -aes128 -passout pass:%password% -out output/root_key.key 4096 
openssl req -x509 -new -nodes -key output/root_key.key -sha256 -days %days% -passin pass:%password% -out output/root_certificate.crt -subj "/C=IL/ST=./L=Rishon Le Zion/O=OpenFTP/OU=./CN=Aviv Kandabi/emailAddress=kandabiaviv@gmail.com" 


openssl genrsa -out output/server.key 4096
openssl req -new -sha256 -key output/server.key -out output/server.csr -subj "/C=IL/ST=./L=Rishon Le Zion/O=OpenFTP Server/OU=./CN=Aviv Kandabi/emailAddress=kandabiaviv@gmail.com"
openssl x509 -req -in output/server.csr -CA output/root_certificate.crt -CAkey output/root_key.key -CAcreateserial -out output/server.crt -days %days% -passin pass:%password% -sha256


openssl genrsa -out output/client.key 4096
openssl req -new -sha256 -key output/client.key -out output/client.csr -subj "/C=IL/ST=./L=Rishon Le Zion/O=OpenFTP Client/OU=./CN=Aviv Kandabi/emailAddress=kandabiaviv@gmail.com"
openssl x509 -req -in output/client.csr -CA output/root_certificate.crt -CAkey output/root_key.key -CAcreateserial -out output/client.crt -days %days% -passin pass:%password% -sha256

cd output

del /f root_certificate.srl
del /f client.csr
del /f server.csr
del /f root_key.key


xcopy "%cd%" "%finalOutputDirectory%" /S /I /Y
echo Generated Certificates in output folder 

cmd /k





