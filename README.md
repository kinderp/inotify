### How to start

* Read cap. 19 in [docs](https://github.com/kinderp/inotify/blob/main/docs/The%20Linux%20Programming%20Interface.pdf)
  
* Read, study and compile [demo_inotify.c](https://github.com/kinderp/inotify/blob/main/example/demo_inotify.c)
  
* Read, study and compile [demo_scandir.c](https://github.com/kinderp/inotify/blob/main/example/demo_scandir.c)

Utilizza l'esempio `demo_scandir.c` (che ritorna le directory e i files presenti in una directory presa in input) per trovare tutte le sotto directory presenti dalla radice fino alla fine dell'alberatura.
Per esempio se la directory test passata in ingresso a `demo_scandir.c` è così composta:
```
test
├───1
│   ├───1.1
│   └───1.2
│       └───1.2.1
└───2
    ├───2.1
    ├───2.2
    ├───2.3
    └───2.4
        └───2.4.1
```

il codice attuale rileverebbe solo le cartelle `1` e `2` che sono le sottodirectory sotto test. Con la ricorsione modificare il codice per rilevare tutte le sottodirectory dell'alberatura fino alle foglie.
Il codice modificato deve quindi rilevare le seguente cartelle: `1`, `1.1`, `1.2.1`, `2`, `2.1`, `2.2`, `2.3`, `2.4`, `2.4.1`
