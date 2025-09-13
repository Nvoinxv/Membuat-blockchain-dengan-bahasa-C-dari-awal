Halo disini gw akan membuat blockchain dari awal dengan bahasa C dan ini pure dari awal kawan kawan. Jadi disini gw akan bahas poin poin penting saja untuk membuat blockchain.

Jadi disini kan ada rangka rangka atau struktur kek gini:
1.blockchain
2.block
3.crypto
4.network
5.transaction
6.utils

Ini gw bahas kerangka di dalam blockchain ya walaupun ada file untuk manajemen JSON yaitu cJSON tetapi ya sudah lah ya itu gw bahas penting aja.

Jadi ringkas nya tuh, blockchain ini kek gabungan dari block yang kita sudah sudah implementasi crypto yang tugas nya itu membuat algortima hash disini gw pakai sha-256 karna terinspirasi dari bitcoin, lalu network sama juga gw pakai PoW atau proof of work, lalu transaksi buat manajemen transaksi atau validasi transaksi dan terakhir utils tuh kek semacam arsip tipe data, fungsi bantu dan kostanta yang akan di pakai di blok dan transaksi.

Untuk menjalankan projek nya, gw pakai terminal windows power shell seperti ini:
``
gcc main.c src/block.c src/blockchain.c src/cJSON.c src/network.c src/crypto.c src/transaction.c src/utils.c -o main.exe

``