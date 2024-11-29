OUTPUT=shadow_client

rm -rf $OUTPUT

g++ -o $OUTPUT main.cpp /home/kali/d3m0n_os_debian/rootfs/usr/share/d3m0n/src/settings.cpp -std=c++17 -I. -lssl -lcrypto  || exit 1

chmod +x $OUTPUT

clear

./$OUTPUT $1 $2 $3