# KASE

## Introduction
Key-Aggregate Searchable Encryption Supporting Conjunctive Queries

## Build

### Compile
make main
### Clean
make clean
### Environmental requirement
install the pbc library
## Use
* ./main a.param 100 4

a.param is the ECC parameter，100 denotes the number of files, and 4 denotes the number of keyword fields.

* After running, the result is as follows：

```html
setup generte time 0.530655 s
------------------------------------------
enc generte time 2.791 s
------------------------------------------
search input 1, exit input 0: 1
------------------------------------------
```

* At this time, choose to input 1 to search, and 0 to exit the program.

* Input the number of documents shared by the data owner to the user (up to 100).

```html
please input you share file number:  10
```
* Input the keyword field for query (up to 4).

```html
please input you wanted keyword field:  2
```
* Input the search keyword (you can choose a keyword from the plaintext file 1004 (it can be the second field or not))

```html
please input Corresponding keyword: ngrlvuxxrg
```
* Continue querying input 1, otherwise input 0. Continuing to query the target document needs to satisfy the conjunctive query.

```html
if continue query, input 1, else input 0:  0
```

```html
trap generte time 0.00343105 s
------------------------------------------
file 2  match	-----> (search result)
search generte time 0.0741453 s
```




