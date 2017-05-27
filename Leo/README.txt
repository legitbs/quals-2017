
Leo


There are two components to the Leo challenge:  leo (64 bit ELF) and module.bin.  leo is the service binary and should be started in the container.  At startup, leo connects to a webserver and downloads module.bin.  By default, the URL for the download is http://localhost/module.bin, but for game day the module must be publically accessible.  Specify the URL with the -u command line option:

leo -u http://<hostname>/module.bin



