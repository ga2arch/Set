# Set

Il progetto richiede la progettazione e realizzazione di una classe che
implementa un Set di elementi generici T. Un Set è una collezione di dati che
non può contenere duplicati: es. S={1,6,4,9,7,10,12}. Implementare il Set in
modo tale che l'accesso tramite un indice all'i-esimo elemento avvenga in
tempo costante.
A parte i metodi essenziali per il suo corretto uso, la classe deve implementare
anche le seguenti funzionalità:
  1. Accesso all'i-esimo elemento tramite operatore [ ]
  2. Aggiunta di un elemento. Se l'elemento esiste nel Set, lanciate una
eccezione.
  3. Rimozione di un elemento. Se l'elemento non esiste nel Set,. lanciate
una eccezione.
  4. Creazione di un Set a partire da una sequenza di dati definita da una
coppia generica di iteratori su tipi Q. Lasciate al compilatore la gestione
della compatibilità tra i tipi attraverso l'uso del cast.
  5. iterator e const_iterator di tipo random.
Implementare inoltre la stampa degli elementi del Set attraverso l'operatore di
stream <<.
Implementare una funzione globale e generica filter_out che, dato un Set
generico S e un predicato booleano generico P, ritorna un nuovo set ottenuto
eliminando da S tutti gli elementi che soddisfano il predicato P.
Utilizzare dove opportuno la gestione delle eccezioni e/o asserzioni. 
