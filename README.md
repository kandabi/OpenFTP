# OpenFTP

[![AppVeyor status](https://ci.appveyor.com/api/projects/status/c4i06722k53tqu16?svg=true)](https://ci.appveyor.com/project/kandabi/openftp)
[![GPL3 license](https://img.shields.io/badge/license-GPL-blue)](https://fsf.org/)

OpenFTP is a file transfer client + server, programmed in C++ / Qt, it uses openssl for securing network connections.  
It aims to be a good alternative to other ftp software, while being feature rich and simple to use.
it does not support connections between non-openftp servers and clients.  

Currently only supports Windows, with macOS and Linux versions planned. 

## Installation

For Windows installation, please download the following software, with the specified versions:

* [Visual Studio 2019](https://visualstudio.microsoft.com/vs/) - The IDE, make sure to install the MSVC2019 v142 Compiler.
* [Qt Framework](https://www.qt.io/download-qt-installer) - The GUI framework, version: 5.14.1, make sure to install the MSVC2017 and MSVC2017_64 Compilers for both 64 and 32 bit support, future Qt versions are likely to break compilation.
* [Qt Visual Studio Extension](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123) - Used to add support for Qt projects inside of Visual Studio.
* [OpenSSL](https://www.openssl.org/) - Used for implementation of secure networking. If you would like only to build the project locally, I would recommend using the Qt online installer, and ticking the openssl checkbox.  
however if you would like to create and sign your own SSL certificates, you must have openssl installed on your system!   
you can build openssl from [here](https://www.openssl.org/source/), or you could download the binaries from a trusted source such as [this website](https://slproweb.com/products/Win32OpenSSL.html). make sure to grab the non-light installer!

Now you should hopefully be able run the solution on your system in both debug and release modes!

## Authors

* **kandabi aviv** - kandabiaviv@gmail.com

## License

This project is licensed under the GPLv3 License - see the [license](LICENSE) file for details
with two exceptions: [Qt LGPLv3](https://doc.qt.io/qt-5/lgpl.html) ,[OpenSSL Apache](https://www.openssl.org/source/apache-license-2.0.txt) 

## Acknowledgments

* [Qt Framework](https://www.qt.io/)
* [OpenSSL](https://www.openssl.org/)
* [SimpleCrypt](https://wiki.qt.io/Simple_encryption_with_SimpleCrypt) - Simple encryption, written by Andre Somers.
* Audio clips made by: [Headphaze](https://freesound.org/people/Headphaze/), [debsound](https://freesound.org/people/debsound/) from [freesound.org](https://freesound.org)
* Icons made by: [freepik](https://www.flaticon.com/authors/freepik), [Kiranshastry](https://www.flaticon.com/authors/kiranshastry) from [flaticon.com](https://flaticon.com)

 