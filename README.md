# search-engine
This is a basic search engine I made for my information retrieval class. It uses flex, a stopwords list, low frequency term filtering, a linear probing hash table for the map file, a linked list for the postings file, and an array accumulator. The weights use the formula: rtf * idf with log base 10.

![home](./images/home.png)
![results](./images/results.png)

# Dependencies
- flex
- php
- gcc
- apache

# Run

## Native
1. install the dependencies
2. build and run the index and query files
3. setup the apache server

## Docker or Podman
### Docker
```bash
docker build -t search_image .
docker run -p 8080:80 --name search_container search_image
```

### Podman
```bash
podman build -t search_image .
podman run -p 8080:80 --name search_container search_image
```
