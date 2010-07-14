#pragma once
// this include is a fix for certain issues in VC7
// see: http://groups.google.com/groups?q=hash_map+visual+.net&hl=en&lr=&ie=UTF-8&oe=UTF-8&selm=uW3FrAuECHA.2672%40tkmsftngp05&rnum=6


// define hash function for strings
struct MyComp
{
enum
 { // parameters for hash table
 bucket_size = 4, // 0 < bucket_size
 min_buckets = 8}; // min_buckets = 2 ^^ N, 0 < N

size_t operator()(const string& s1) const
 { // hash string s1 to size_t value
 const unsigned char *p = (const unsigned char *)s1.c_str();
 size_t hashval = 0;

 for (size_t n = s1.size(); 0 < n; --n)
  hashval += *p++; // or whatever
 return (hashval);
 }

bool operator()(const string &s1, const string &s2) const
 { // test if s1 ordered before s2
 return (s1 < s2);
 }
};

