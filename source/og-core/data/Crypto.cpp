/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "Crypto.h"
#include <cassert>

#define	ZeroStruct(s)		{memset((void *)(&(s)), 0, sizeof(s));}

//-----------------------------------------------------------------------------

const int Crypto::prime_table[] = 
{
   2,     3,     5,     7,     11,    13,    17,    19,    23,    29, 
   31,    37,    41,    43,    47,    53,    59,    61,    67,    71, 
   73,    79,    83,    89,    97,    101,   103,   107,   109,   113, 
   127,   131,   137,   139,   149,   151,   157,   163,   167,   173, 
   179,   181,   191,   193,   197,   199,   211,   223,   227,   229, 
   233,   239,   241,   251,   257,   263,   269,   271,   277,   281, 
   283,   293,   307,   311,   313,   317,   331,   337,   347,   349, 
   353,   359,   367,   373,   379,   383,   389,   397,   401,   409, 
   419,   421,   431,   433,   439,   443,   449,   457,   461,   463, 
   467,   479,   487,   491,   499,   503,   509,   521,   523,   541, 
   547,   557,   563,   569,   571,   577,   587,   593,   599,   601, 
   607,   613,   617,   619,   631,   641,   643,   647,   653,   659, 
   661,   673,   677,   683,   691,   701,   709,   719,   727,   733, 
   739,   743,   751,   757,   761,   769,   773,   787,   797,   809, 
   811,   821,   823,   827,   829,   839,   853,   857,   859,   863, 
   877,   881,   883,   887,   907,   911,   919,   929,   937,   941, 
   947,   953,   967,   971,   977,   983,   991,   997,   1009,  1013, 
   1019,  1021,  1031,  1033,  1039,  1049,  1051,  1061,  1063,  1069, 
   1087,  1091,  1093,  1097,  1103,  1109,  1117,  1123,  1129,  1151, 
   1153,  1163,  1171,  1181,  1187,  1193,  1201,  1213,  1217,  1223, 
   1229,  1231,  1237,  1249,  1259,  1277,  1279,  1283,  1289,  1291, 
   1297,  1301,  1303,  1307,  1319,  1321,  1327,  1361,  1367,  1373, 
   1381,  1399,  1409,  1423,  1427,  1429,  1433,  1439,  1447,  1451, 
   1453,  1459,  1471,  1481,  1483,  1487,  1489,  1493,  1499,  1511, 
   1523,  1531,  1543,  1549,  1553,  1559,  1567,  1571,  1579,  1583, 
   1597,  1601,  1607,  1609,  1613,  1619,  1621,  1627,  1637,  1657, 
   1663,  1667,  1669,  1693,  1697,  1699,  1709,  1721,  1723,  1733, 
   1741,  1747,  1753,  1759,  1777,  1783,  1787,  1789,  1801,  1811,
   1823,  1831,  1847,  1861,  1867,  1871,  1873,  1877,  1879,  1889, 
   1901,  1907,  1913,  1931,  1933,  1949,  1951,  1973,  1979,  1987, 
   1993,  1997,  1999,  2003,  2011,  2017,  2027,  2029,  2039,  2053,
   2063,  2069,  2081,  2083,  2087,  2089,  2099,  2111,  2113,  2129,
   2131,  2137,  2141,  2143,  2153,  2161,  2179,  2203,  2207,  2213,
   2221,  2237,  2239,  2243,  2251,  2267,  2269,  2273,  2281,  2287,
   2293,  2297,  2309,  2311,  2333,  2339,  2341,  2347,  2351,  2357,
   2371,  2377,  2381,  2383,  2389,  2393,  2399,  2411,  2417,  2423,
   2437,  2441,  2447,  2459,  2467,  2473,  2477,  2503,  2521,  2531,
   2539,  2543,  2549,  2551,  2557,  2579,  2591,  2593,  2609,  2617,
   2621,  2633,  2647,  2657,  2659,  2663,  2671,  2677,  2683,  2687,
   2689,  2693,  2699,  2707,  2711,  2713,  2719,  2729,  2731,  2741,
   2749,  2753,  2767,  2777,  2789,  2791,  2797,  2801,  2803,  2819,
   2833,  2837,  2843,  2851,  2857,  2861,  2879,  2887,  2897,  2903,
   2909,  2917,  2927,  2939,  2953,  2957,  2963,  2969,  2971,  2999,
   3001,  3011,  3019,  3023,  3037,  3041,  3049,  3061,  3067,  3079,
   3083,  3089,  3109,  3119,  3121,  3137,  3163,  3167,  3169,  3181,
   3187,  3191,  3203,  3209,  3217,  3221,  3229,  3251,  3253,  3257,
   3259,  3271,  3299,  3301,  3307,  3313,  3319,  3323,  3329,  3331,
   3343,  3347,  3359,  3361,  3371,  3373,  3389,  3391,  3407,  3413,
   3433,  3449,  3457,  3461,  3463,  3467,  3469,  3491,  3499,  3511,
   3517,  3527,  3529,  3533,  3539,  3541,  3547,  3557,  3559,  3571,
   3581,  3583,  3593,  3607,  3613,  3617,  3623,  3631,  3637,  3643,
   3659,  3671,  3673,  3677,  3691,  3697,  3701,  3709,  3719,  3727,
   3733,  3739,  3761,  3767,  3769,  3779,  3793,  3797,  3803,  3821,
   3823,  3833,  3847,  3851,  3853,  3863,  3877,  3881,  3889,  3907,
   3911,  3917,  3919,  3923,  3929,  3931,  3943,  3947,  3967,  3989,
   4001,  4003,  4007,  4013,  4019,  4021,  4027,  4049,  4051,  4057,
   4073,  4079,  4091,  4093,  4099,  4111,  4127,  4129,  4133,  4139,
   4153,  4157,  4159,  4177,  4201,  4211,  4217,  4219,  4229,  4231,
   4241,  4243,  4253,  4259,  4261,  4271,  4273,  4283,  4289,  4297,
   4327,  4337,  4339,  4349,  4357,  4363,  4373,  4391,  4397,  4409,
   4421,  4423,  4441,  4447,  4451,  4457,  4463,  4481,  4483,  4493,
   4507,  4513,  4517,  4519,  4523,  4547,  4549,  4561,  4567,  4583,
   4591,  4597,  4603,  4621,  4637,  4639,  4643,  4649,  4651,  4657,
   4663,  4673,  4679,  4691,  4703,  4721,  4723,  4729,  4733,  4751,
   4759,  4783,  4787,  4789,  4793,  4799,  4801,  4813,  4817,  4831,
   4861,  4871,  4877,  4889,  4903,  4909,  4919,  4931,  4933,  4937,
   4943,  4951,  4957,  4967,  4969,  4973,  4987,  4993,  4999,  5003,
   5009,  5011,  5021,  5023,  5039,  5051,  5059,  5077,  5081,  5087,
   5099,  5101,  5107,  5113,  5119,  5147,  5153,  5167,  5171,  5179,
   5189,  5197,  5209,  5227,  5231,  5233,  5237,  5261,  5273,  5279,
   5281,  5297,  5303,  5309,  5323,  5333,  5347,  5351,  5381,  5387,
   5393,  5399,  5407,  5413,  5417,  5419,  5431,  5437,  5441,  5443,
   5449,  5471,  5477,  5479,  5483,  5501,  5503,  5507,  5519,  5521,
   5527,  5531,  5557,  5563,  5569,  5573,  5581,  5591,  5623,  5639,
   5641,  5647,  5651,  5653,  5657,  5659,  5669,  5683,  5689,  5693,
   5701,  5711,  5717,  5737,  5741,  5743,  5749,  5779,  5783,  5791,
   5801,  5807,  5813,  5821,  5827,  5839,  5843,  5849,  5851,  5857,
   5861,  5867,  5869,  5879,  5881,  5897,  5903,  5923,  5927,  5939,
   5953,  5981,  5987,  6007,  6011,  6029,  6037,  6043,  6047,  6053,
   6067,  6073,  6079,  6089,  6091,  6101,  6113,  6121,  6131,  6133,
   6143,  6151,  6163,  6173,  6197,  6199,  6203,  6211,  6217,  6221,
   6229,  6247,  6257,  6263,  6269,  6271,  6277,  6287,  6299,  6301,
   6311,  6317,  6323,  6329,  6337,  6343,  6353,  6359,  6361,  6367,
   6373,  6379,  6389,  6397,  6421,  6427,  6449,  6451,  6469,  6473,
   6481,  6491,  6521,  6529,  6547,  6551,  6553,  6563,  6569,  6571,
   6577,  6581,  6599,  6607,  6619,  6637,  6653,  6659,  6661,  6673,
   6679,  6689,  6691,  6701,  6703,  6709,  6719,  6733,  6737,  6761,
   6763,  6779,  6781,  6791,  6793,  6803,  6823,  6827,  6829,  6833,
   6841,  6857,  6863,  6869,  6871,  6883,  6899,  6907,  6911,  6917,
   6947,  6949,  6959,  6961,  6967,  6971,  6977,  6983,  6991,  6997,
   7001,  7013,  7019,  7027,  7039,  7043,  7057,  7069,  7079,  7103,
   7109,  7121,  7127,  7129,  7151,  7159,  7177,  7187,  7193,  7207,
   7211,  7213,  7219,  7229,  7237,  7243,  7247,  7253,  7283,  7297,
   7307,  7309,  7321,  7331,  7333,  7349,  7351,  7369,  7393,  7411,
   7417,  7433,  7451,  7457,  7459,  7477,  7481,  7487,  7489,  7499,
   7507,  7517,  7523,  7529,  7537,  7541,  7547,  7549,  7559,  7561,
   7573,  7577,  7583,  7589,  7591,  7603,  7607,  7621,  7639,  7643,
   7649,  7669,  7673,  7681,  7687,  7691,  7699,  7703,  7717,  7723,
   7727,  7741,  7753,  7757,  7759,  7789,  7793,  7817,  7823,  7829,
   7841,  7853,  7867,  7873,  7877,  7879,  7883,  7901,  7907,  7919,
   7927,  7933,  7937,  7949,  7951,  7963,  7993,  8009,  8011,  8017,
   8039,  8053,  8059,  8069,  8081,  8087,  8089,  8093,  8101,  8111,
   8117,  8123,  8147,  8161,  8167,  8171,  8179,  8191,  8209,  8219,
   8221,  8231,  8233,  8237,  8243,  8263,  8269,  8273,  8287,  8291,
   8293,  8297,  8311,  8317,  8329,  8353,  8363,  8369,  8377,  8387,
   8389,  8419,  8423,  8429,  8431,  8443,  8447,  8461,  8467,  8501,
   8513,  8521,  8527,  8537,  8539,  8543,  8563,  8573,  8581,  8597,
   8599,  8609,  8623,  8627,  8629,  8641,  8647,  8663,  8669,  8677,
   8681,  8689,  8693,  8699,  8707,  8713,  8719,  8731,  8737,  8741,
   8747,  8753,  8761,  8779,  8783,  8803,  8807,  8819,  8821,  8831,
   8837,  8839,  8849,  8861,  8863,  8867,  8887,  8893,  8923,  8929,
   8933,  8941,  8951,  8963,  8969,  8971,  8999,  9001,  9007,  9011,
   9013,  9029,  9041,  9043,  9049,  9059,  9067,  9091,  9103,  9109,
   9127,  9133,  9137,  9151,  9157,  9161,  9173,  9181,  9187,  9199,
   9203,  9209,  9221,  9227,  9239,  9241,  9257,  9277,  9281,  9283,
   9293,  9311,  9319,  9323,  9337,  9341,  9343,  9349,  9371,  9377,
   9391,  9397,  9403,  9413,  9419,  9421,  9431,  9433,  9437,  9439,
   9461,  9463,  9467,  9473,  9479,  9491,  9497,  9511,  9521,  9533,
   9539,  9547,  9551,  9587,  9601,  9613,  9619,  9623,  9629,  9631,
   9643,  9649,  9661,  9677,  9679,  9689,  9697,  9719,  9721,  9733,
   9739,  9743,  9749,  9767,  9769,  9781,  9787,  9791,  9803,  9811,
   9817,  9829,  9833,  9839,  9851,  9857,  9859,  9871,  9883,  9887,
   9901,  9907,  9923,  9929,  9931,  9941,  9949,  9967,  9973
};

//-----------------------------------------------------------------------------

int Crypto::smallPrime(int n) 
{
   assert(n < numSmallPrimes() && n >= 0);

   // From:
   // http://aleph0.clarku.edu/~djoyce/numbers/primes.html

                                                              
      return prime_table[n];
}

//-----------------------------------------------------------------------------

int Crypto::numSmallPrimes() 
{
   return sizeof(prime_table)/sizeof(int);
}

//-----------------------------------------------------------------------------

unsigned int Crypto::crc32(const void* byte, size_t numBytes) 
{
   static const unsigned int crc32Table[256] = 
   {
      0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
      0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
      0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
      0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
      0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
      0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
      0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
      0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
      0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
      0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
      0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
      0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
      0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
      0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
      0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
      0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

      0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
      0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
      0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
      0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
      0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
      0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
      0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
      0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
      0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
      0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
      0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
      0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
      0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
      0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
      0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
      0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

      0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
      0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
      0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
      0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
      0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
      0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
      0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
      0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
      0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
      0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
      0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
      0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
      0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
      0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
      0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
      0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

      0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
      0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
      0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
      0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
      0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
      0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
      0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
      0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
      0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
      0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
      0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
      0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
      0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
      0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
      0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
      0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
   };

   // By definition, initialize to all binary 1's
   unsigned int value = 0xFFFFFFFF;

   for (size_t i = 0; i < numBytes; ++i) 
   {
      value = (value >> 8 ) ^ crc32Table[static_cast<const unsigned char*>(byte)[i] ^ (value & 0xFF)];
   }

   return value;
}

//-----------------------------------------------------------------------------

// crc64

uint64 Crypto::crc64(const void* byte, size_t numBytes)
{
   static const uint64 crc64Table[256] =
   {
      ULLCONST(0x0000000000000000), ULLCONST(0x42f0e1eba9ea3693),
      ULLCONST(0x85e1c3d753d46d26), ULLCONST(0xc711223cfa3e5bb5),
      ULLCONST(0x493366450e42ecdf), ULLCONST(0x0bc387aea7a8da4c),
      ULLCONST(0xccd2a5925d9681f9), ULLCONST(0x8e224479f47cb76a),
      ULLCONST(0x9266cc8a1c85d9be), ULLCONST(0xd0962d61b56fef2d),
      ULLCONST(0x17870f5d4f51b498), ULLCONST(0x5577eeb6e6bb820b),
      ULLCONST(0xdb55aacf12c73561), ULLCONST(0x99a54b24bb2d03f2),
      ULLCONST(0x5eb4691841135847), ULLCONST(0x1c4488f3e8f96ed4),
      ULLCONST(0x663d78ff90e185ef), ULLCONST(0x24cd9914390bb37c),
      ULLCONST(0xe3dcbb28c335e8c9), ULLCONST(0xa12c5ac36adfde5a),
      ULLCONST(0x2f0e1eba9ea36930), ULLCONST(0x6dfeff5137495fa3),
      ULLCONST(0xaaefdd6dcd770416), ULLCONST(0xe81f3c86649d3285),
      ULLCONST(0xf45bb4758c645c51), ULLCONST(0xb6ab559e258e6ac2),
      ULLCONST(0x71ba77a2dfb03177), ULLCONST(0x334a9649765a07e4),
      ULLCONST(0xbd68d2308226b08e), ULLCONST(0xff9833db2bcc861d),
      ULLCONST(0x388911e7d1f2dda8), ULLCONST(0x7a79f00c7818eb3b),
      ULLCONST(0xcc7af1ff21c30bde), ULLCONST(0x8e8a101488293d4d),
      ULLCONST(0x499b3228721766f8), ULLCONST(0x0b6bd3c3dbfd506b),
      ULLCONST(0x854997ba2f81e701), ULLCONST(0xc7b97651866bd192),
      ULLCONST(0x00a8546d7c558a27), ULLCONST(0x4258b586d5bfbcb4),
      ULLCONST(0x5e1c3d753d46d260), ULLCONST(0x1cecdc9e94ace4f3),
      ULLCONST(0xdbfdfea26e92bf46), ULLCONST(0x990d1f49c77889d5),
      ULLCONST(0x172f5b3033043ebf), ULLCONST(0x55dfbadb9aee082c),
      ULLCONST(0x92ce98e760d05399), ULLCONST(0xd03e790cc93a650a),
      ULLCONST(0xaa478900b1228e31), ULLCONST(0xe8b768eb18c8b8a2),
      ULLCONST(0x2fa64ad7e2f6e317), ULLCONST(0x6d56ab3c4b1cd584),
      ULLCONST(0xe374ef45bf6062ee), ULLCONST(0xa1840eae168a547d),
      ULLCONST(0x66952c92ecb40fc8), ULLCONST(0x2465cd79455e395b),
      ULLCONST(0x3821458aada7578f), ULLCONST(0x7ad1a461044d611c),
      ULLCONST(0xbdc0865dfe733aa9), ULLCONST(0xff3067b657990c3a),
      ULLCONST(0x711223cfa3e5bb50), ULLCONST(0x33e2c2240a0f8dc3),
      ULLCONST(0xf4f3e018f031d676), ULLCONST(0xb60301f359dbe0e5),
      ULLCONST(0xda050215ea6c212f), ULLCONST(0x98f5e3fe438617bc),
      ULLCONST(0x5fe4c1c2b9b84c09), ULLCONST(0x1d14202910527a9a),
      ULLCONST(0x93366450e42ecdf0), ULLCONST(0xd1c685bb4dc4fb63),
      ULLCONST(0x16d7a787b7faa0d6), ULLCONST(0x5427466c1e109645),
      ULLCONST(0x4863ce9ff6e9f891), ULLCONST(0x0a932f745f03ce02),
      ULLCONST(0xcd820d48a53d95b7), ULLCONST(0x8f72eca30cd7a324),
      ULLCONST(0x0150a8daf8ab144e), ULLCONST(0x43a04931514122dd),
      ULLCONST(0x84b16b0dab7f7968), ULLCONST(0xc6418ae602954ffb),
      ULLCONST(0xbc387aea7a8da4c0), ULLCONST(0xfec89b01d3679253),
      ULLCONST(0x39d9b93d2959c9e6), ULLCONST(0x7b2958d680b3ff75),
      ULLCONST(0xf50b1caf74cf481f), ULLCONST(0xb7fbfd44dd257e8c),
      ULLCONST(0x70eadf78271b2539), ULLCONST(0x321a3e938ef113aa),
      ULLCONST(0x2e5eb66066087d7e), ULLCONST(0x6cae578bcfe24bed),
      ULLCONST(0xabbf75b735dc1058), ULLCONST(0xe94f945c9c3626cb),
      ULLCONST(0x676dd025684a91a1), ULLCONST(0x259d31cec1a0a732),
      ULLCONST(0xe28c13f23b9efc87), ULLCONST(0xa07cf2199274ca14),
      ULLCONST(0x167ff3eacbaf2af1), ULLCONST(0x548f120162451c62),
      ULLCONST(0x939e303d987b47d7), ULLCONST(0xd16ed1d631917144),
      ULLCONST(0x5f4c95afc5edc62e), ULLCONST(0x1dbc74446c07f0bd),
      ULLCONST(0xdaad56789639ab08), ULLCONST(0x985db7933fd39d9b),
      ULLCONST(0x84193f60d72af34f), ULLCONST(0xc6e9de8b7ec0c5dc),
      ULLCONST(0x01f8fcb784fe9e69), ULLCONST(0x43081d5c2d14a8fa),
      ULLCONST(0xcd2a5925d9681f90), ULLCONST(0x8fdab8ce70822903),
      ULLCONST(0x48cb9af28abc72b6), ULLCONST(0x0a3b7b1923564425),
      ULLCONST(0x70428b155b4eaf1e), ULLCONST(0x32b26afef2a4998d),
      ULLCONST(0xf5a348c2089ac238), ULLCONST(0xb753a929a170f4ab),
      ULLCONST(0x3971ed50550c43c1), ULLCONST(0x7b810cbbfce67552),
      ULLCONST(0xbc902e8706d82ee7), ULLCONST(0xfe60cf6caf321874),
      ULLCONST(0xe224479f47cb76a0), ULLCONST(0xa0d4a674ee214033),
      ULLCONST(0x67c58448141f1b86), ULLCONST(0x253565a3bdf52d15),
      ULLCONST(0xab1721da49899a7f), ULLCONST(0xe9e7c031e063acec),
      ULLCONST(0x2ef6e20d1a5df759), ULLCONST(0x6c0603e6b3b7c1ca),
      ULLCONST(0xf6fae5c07d3274cd), ULLCONST(0xb40a042bd4d8425e),
      ULLCONST(0x731b26172ee619eb), ULLCONST(0x31ebc7fc870c2f78),
      ULLCONST(0xbfc9838573709812), ULLCONST(0xfd39626eda9aae81),
      ULLCONST(0x3a28405220a4f534), ULLCONST(0x78d8a1b9894ec3a7),
      ULLCONST(0x649c294a61b7ad73), ULLCONST(0x266cc8a1c85d9be0),
      ULLCONST(0xe17dea9d3263c055), ULLCONST(0xa38d0b769b89f6c6),
      ULLCONST(0x2daf4f0f6ff541ac), ULLCONST(0x6f5faee4c61f773f),
      ULLCONST(0xa84e8cd83c212c8a), ULLCONST(0xeabe6d3395cb1a19),
      ULLCONST(0x90c79d3fedd3f122), ULLCONST(0xd2377cd44439c7b1),
      ULLCONST(0x15265ee8be079c04), ULLCONST(0x57d6bf0317edaa97),
      ULLCONST(0xd9f4fb7ae3911dfd), ULLCONST(0x9b041a914a7b2b6e),
      ULLCONST(0x5c1538adb04570db), ULLCONST(0x1ee5d94619af4648),
      ULLCONST(0x02a151b5f156289c), ULLCONST(0x4051b05e58bc1e0f),
      ULLCONST(0x87409262a28245ba), ULLCONST(0xc5b073890b687329),
      ULLCONST(0x4b9237f0ff14c443), ULLCONST(0x0962d61b56fef2d0),
      ULLCONST(0xce73f427acc0a965), ULLCONST(0x8c8315cc052a9ff6),
      ULLCONST(0x3a80143f5cf17f13), ULLCONST(0x7870f5d4f51b4980),
      ULLCONST(0xbf61d7e80f251235), ULLCONST(0xfd913603a6cf24a6),
      ULLCONST(0x73b3727a52b393cc), ULLCONST(0x31439391fb59a55f),
      ULLCONST(0xf652b1ad0167feea), ULLCONST(0xb4a25046a88dc879),
      ULLCONST(0xa8e6d8b54074a6ad), ULLCONST(0xea16395ee99e903e),
      ULLCONST(0x2d071b6213a0cb8b), ULLCONST(0x6ff7fa89ba4afd18),
      ULLCONST(0xe1d5bef04e364a72), ULLCONST(0xa3255f1be7dc7ce1),
      ULLCONST(0x64347d271de22754), ULLCONST(0x26c49cccb40811c7),
      ULLCONST(0x5cbd6cc0cc10fafc), ULLCONST(0x1e4d8d2b65facc6f),
      ULLCONST(0xd95caf179fc497da), ULLCONST(0x9bac4efc362ea149),
      ULLCONST(0x158e0a85c2521623), ULLCONST(0x577eeb6e6bb820b0),
      ULLCONST(0x906fc95291867b05), ULLCONST(0xd29f28b9386c4d96),
      ULLCONST(0xcedba04ad0952342), ULLCONST(0x8c2b41a1797f15d1),
      ULLCONST(0x4b3a639d83414e64), ULLCONST(0x09ca82762aab78f7),
      ULLCONST(0x87e8c60fded7cf9d), ULLCONST(0xc51827e4773df90e),
      ULLCONST(0x020905d88d03a2bb), ULLCONST(0x40f9e43324e99428),
      ULLCONST(0x2cffe7d5975e55e2), ULLCONST(0x6e0f063e3eb46371),
      ULLCONST(0xa91e2402c48a38c4), ULLCONST(0xebeec5e96d600e57),
      ULLCONST(0x65cc8190991cb93d), ULLCONST(0x273c607b30f68fae),
      ULLCONST(0xe02d4247cac8d41b), ULLCONST(0xa2dda3ac6322e288),
      ULLCONST(0xbe992b5f8bdb8c5c), ULLCONST(0xfc69cab42231bacf),
      ULLCONST(0x3b78e888d80fe17a), ULLCONST(0x7988096371e5d7e9),
      ULLCONST(0xf7aa4d1a85996083), ULLCONST(0xb55aacf12c735610),
      ULLCONST(0x724b8ecdd64d0da5), ULLCONST(0x30bb6f267fa73b36),
      ULLCONST(0x4ac29f2a07bfd00d), ULLCONST(0x08327ec1ae55e69e),
      ULLCONST(0xcf235cfd546bbd2b), ULLCONST(0x8dd3bd16fd818bb8),
      ULLCONST(0x03f1f96f09fd3cd2), ULLCONST(0x41011884a0170a41),
      ULLCONST(0x86103ab85a2951f4), ULLCONST(0xc4e0db53f3c36767),
      ULLCONST(0xd8a453a01b3a09b3), ULLCONST(0x9a54b24bb2d03f20),
      ULLCONST(0x5d45907748ee6495), ULLCONST(0x1fb5719ce1045206),
      ULLCONST(0x919735e51578e56c), ULLCONST(0xd367d40ebc92d3ff),
      ULLCONST(0x1476f63246ac884a), ULLCONST(0x568617d9ef46bed9),
      ULLCONST(0xe085162ab69d5e3c), ULLCONST(0xa275f7c11f7768af),
      ULLCONST(0x6564d5fde549331a), ULLCONST(0x279434164ca30589),
      ULLCONST(0xa9b6706fb8dfb2e3), ULLCONST(0xeb46918411358470),
      ULLCONST(0x2c57b3b8eb0bdfc5), ULLCONST(0x6ea7525342e1e956),
      ULLCONST(0x72e3daa0aa188782), ULLCONST(0x30133b4b03f2b111),
      ULLCONST(0xf7021977f9cceaa4), ULLCONST(0xb5f2f89c5026dc37),
      ULLCONST(0x3bd0bce5a45a6b5d), ULLCONST(0x79205d0e0db05dce),
      ULLCONST(0xbe317f32f78e067b), ULLCONST(0xfcc19ed95e6430e8),
      ULLCONST(0x86b86ed5267cdbd3), ULLCONST(0xc4488f3e8f96ed40),
      ULLCONST(0x0359ad0275a8b6f5), ULLCONST(0x41a94ce9dc428066),
      ULLCONST(0xcf8b0890283e370c), ULLCONST(0x8d7be97b81d4019f),
      ULLCONST(0x4a6acb477bea5a2a), ULLCONST(0x089a2aacd2006cb9),
      ULLCONST(0x14dea25f3af9026d), ULLCONST(0x562e43b4931334fe),
      ULLCONST(0x913f6188692d6f4b), ULLCONST(0xd3cf8063c0c759d8),
      ULLCONST(0x5dedc41a34bbeeb2), ULLCONST(0x1f1d25f19d51d821),
      ULLCONST(0xd80c07cd676f8394), ULLCONST(0x9afce626ce85b507)
   };

   uint64 value = ULLCONST(0xFFFFFFFFFFFFFFFF); 

   for (size_t i = 0; i < numBytes; i++) 
   {
      value = crc64Table[(unsigned char)(value >> 56) ^ static_cast<const unsigned char*>(byte)[i]] ^ (value << 8);
   }

   return value;

}

//-----------------------------------------------------------------------------

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s 
{
   md5_word_t count[2];	/* message length in bits, lsw first */
   md5_word_t abcd[4];		/* digest buffer */
   md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

#ifdef __cplusplus
extern "C" 
{
#endif

   /* Initialize the algorithm. */
   static void md5_init(md5_state_t *pms);

   /* Append a string to the message. */
   static void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);

   /* Finish the message and return the digest. */
   void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
#ifdef __cplusplus
}    
#endif

//-----------------------------------------------------------------------------

MD5Hash Crypto::md5(const void* data, size_t n) 
{
   md5_state_t state;
   md5_init(&state);
   md5_append(&state, (const unsigned char*)data, (int)n);

   MD5Hash h;
   md5_finish(&state, &(h[0]));
   return h;
}

/*
Copyright (C) 1999, 2000, 2002 Aladdin Enterprises.  All rights reserved.

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

L. Peter Deutsch
ghost@aladdin.com

*/
/* $Id: Crypto_md5.cpp,v 1.3 2006/04/14 22:48:35 morgan3d Exp $ */
/*
Independent implementation of MD5 (RFC 1321).

This code implements the MD5 Algorithm defined in RFC 1321, whose
text is available at
http://www.ietf.org/rfc/rfc1321.txt
The code is derived from the text of the RFC, including the test suite
(section A.5) but excluding the rest of Appendix A.  It does not include
any code or documentation that is identified in the RFC as being
copyrighted.

The original and principal author of md5.c is L. Peter Deutsch
<ghost@aladdin.com>.  Other authors are noted in the change history
that follows (in reverse chronological order):

2002-04-13 lpd Clarified derivation from RFC 1321; now handles byte order
either statically or dynamically; added missing #include <string.h>
in library.
2002-03-11 lpd Corrected argument list for main(), and added int return
type, in test program and T value program.
2002-02-21 lpd Added missing #include <stdio.h> in test program.
2000-07-03 lpd Patched to eliminate warnings about "constant is
unsigned in ANSI C, signed in traditional"; made test program
self-checking.
1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5).
1999-05-03 lpd Original version.
*/

/*
* This package supports both compile-time and run-time determination of CPU
* byte order.  If ARCH_IS_BIG_ENDIAN is defined as 0, the code will be
* compiled to run only on little-endian CPUs; if ARCH_IS_BIG_ENDIAN is
* defined as non-zero, the code will be compiled to run only on big-endian
* CPUs; if ARCH_IS_BIG_ENDIAN is not defined, the code will be compiled to
* run on either big- or little-endian CPUs, but will run slightly less
* efficiently on either one than if ARCH_IS_BIG_ENDIAN is defined.
*/

#ifdef OS_WINDOWS
//#undef BYTE_ORDER /* 1 = big-endian, -1 = little-endian, 0 = unknown */
//#ifdef ARCH_IS_BIG_ENDIAN
//#  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
//#else
#  define BYTE_ORDER 0
//#endif
#endif

#define T_MASK ((md5_word_t)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)


static void md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
{
   md5_word_t
      a = pms->abcd[0], b = pms->abcd[1],
      c = pms->abcd[2], d = pms->abcd[3];
   md5_word_t t;
#if BYTE_ORDER > 0
   /* Define storage only for big-endian CPUs. */
   md5_word_t X[16];
#else
   /* Define storage for little-endian or both types of CPUs. */
   md5_word_t xbuf[16];
   const md5_word_t *X;
#endif

   {
#if BYTE_ORDER == 0
      /*
      * Determine dynamically whether this is a big-endian or
      * little-endian machine, since we can use a more efficient
      * algorithm on the latter.
      */
      static const int w = 1;

      if (*((const md5_byte_t *)&w)) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0		/* little-endian */
      {
         /*
         * On little-endian machines, we can process properly aligned
         * data without copying it.
         */
         if (!((data - (const md5_byte_t *)0) & 3)) 
         {
            /* data are properly aligned */
            X = (const md5_word_t *)data;
         } else {
            /* not aligned */
            memcpy(xbuf, data, 64);
            X = xbuf;
         }
      }
#endif
#if BYTE_ORDER == 0
      else			/* dynamic big-endian */
#endif
#if BYTE_ORDER >= 0		/* big-endian */
      {
         /*
         * On big-endian machines, we must arrange the bytes in the
         * right order.
         */
         const md5_byte_t *xp = data;
         int i;

#  if BYTE_ORDER == 0
         X = xbuf;		/* (dynamic only) */
#  else
#    define xbuf X		/* (static only) */
#  endif
         for (i = 0; i < 16; ++i, xp += 4)
            xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
      }
#endif
   }

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

   /* Round 1. */
   /* Let [abcd k s i] denote the operation
   a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
   t = a + F(b,c,d) + X[k] + Ti;\
   a = ROTATE_LEFT(t, s) + b
   /* Do the following 16 operations. */
   SET(a, b, c, d,  0,  7,  T1);
   SET(d, a, b, c,  1, 12,  T2);
   SET(c, d, a, b,  2, 17,  T3);
   SET(b, c, d, a,  3, 22,  T4);
   SET(a, b, c, d,  4,  7,  T5);
   SET(d, a, b, c,  5, 12,  T6);
   SET(c, d, a, b,  6, 17,  T7);
   SET(b, c, d, a,  7, 22,  T8);
   SET(a, b, c, d,  8,  7,  T9);
   SET(d, a, b, c,  9, 12, T10);
   SET(c, d, a, b, 10, 17, T11);
   SET(b, c, d, a, 11, 22, T12);
   SET(a, b, c, d, 12,  7, T13);
   SET(d, a, b, c, 13, 12, T14);
   SET(c, d, a, b, 14, 17, T15);
   SET(b, c, d, a, 15, 22, T16);
#undef SET

   /* Round 2. */
   /* Let [abcd k s i] denote the operation
   a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
   t = a + G(b,c,d) + X[k] + Ti;\
   a = ROTATE_LEFT(t, s) + b
   /* Do the following 16 operations. */
   SET(a, b, c, d,  1,  5, T17);
   SET(d, a, b, c,  6,  9, T18);
   SET(c, d, a, b, 11, 14, T19);
   SET(b, c, d, a,  0, 20, T20);
   SET(a, b, c, d,  5,  5, T21);
   SET(d, a, b, c, 10,  9, T22);
   SET(c, d, a, b, 15, 14, T23);
   SET(b, c, d, a,  4, 20, T24);
   SET(a, b, c, d,  9,  5, T25);
   SET(d, a, b, c, 14,  9, T26);
   SET(c, d, a, b,  3, 14, T27);
   SET(b, c, d, a,  8, 20, T28);
   SET(a, b, c, d, 13,  5, T29);
   SET(d, a, b, c,  2,  9, T30);
   SET(c, d, a, b,  7, 14, T31);
   SET(b, c, d, a, 12, 20, T32);
#undef SET

   /* Round 3. */
   /* Let [abcd k s t] denote the operation
   a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
   t = a + H(b,c,d) + X[k] + Ti;\
   a = ROTATE_LEFT(t, s) + b
   /* Do the following 16 operations. */
   SET(a, b, c, d,  5,  4, T33);
   SET(d, a, b, c,  8, 11, T34);
   SET(c, d, a, b, 11, 16, T35);
   SET(b, c, d, a, 14, 23, T36);
   SET(a, b, c, d,  1,  4, T37);
   SET(d, a, b, c,  4, 11, T38);
   SET(c, d, a, b,  7, 16, T39);
   SET(b, c, d, a, 10, 23, T40);
   SET(a, b, c, d, 13,  4, T41);
   SET(d, a, b, c,  0, 11, T42);
   SET(c, d, a, b,  3, 16, T43);
   SET(b, c, d, a,  6, 23, T44);
   SET(a, b, c, d,  9,  4, T45);
   SET(d, a, b, c, 12, 11, T46);
   SET(c, d, a, b, 15, 16, T47);
   SET(b, c, d, a,  2, 23, T48);
#undef SET

   /* Round 4. */
   /* Let [abcd k s t] denote the operation
   a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
   t = a + I(b,c,d) + X[k] + Ti;\
   a = ROTATE_LEFT(t, s) + b
   /* Do the following 16 operations. */
   SET(a, b, c, d,  0,  6, T49);
   SET(d, a, b, c,  7, 10, T50);
   SET(c, d, a, b, 14, 15, T51);
   SET(b, c, d, a,  5, 21, T52);
   SET(a, b, c, d, 12,  6, T53);
   SET(d, a, b, c,  3, 10, T54);
   SET(c, d, a, b, 10, 15, T55);
   SET(b, c, d, a,  1, 21, T56);
   SET(a, b, c, d,  8,  6, T57);
   SET(d, a, b, c, 15, 10, T58);
   SET(c, d, a, b,  6, 15, T59);
   SET(b, c, d, a, 13, 21, T60);
   SET(a, b, c, d,  4,  6, T61);
   SET(d, a, b, c, 11, 10, T62);
   SET(c, d, a, b,  2, 15, T63);
   SET(b, c, d, a,  9, 21, T64);
#undef SET

   /* Then perform the following additions. (That is increment each
   of the four registers by the value it had before this block
   was started.) */
   pms->abcd[0] += a;
   pms->abcd[1] += b;
   pms->abcd[2] += c;
   pms->abcd[3] += d;
}

//-----------------------------------------------------------------------------

void md5_init(md5_state_t *pms)
{
   pms->count[0] = pms->count[1] = 0;
   pms->abcd[0] = 0x67452301;
   pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
   pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
   pms->abcd[3] = 0x10325476;
}

//-----------------------------------------------------------------------------

void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
   const md5_byte_t *p = data;
   int left = nbytes;
   int offset = (pms->count[0] >> 3) & 63;
   md5_word_t nbits = (md5_word_t)(nbytes << 3);

   if (nbytes <= 0)
      return;

   /* Update the message length. */
   pms->count[1] += nbytes >> 29;
   pms->count[0] += nbits;
   if (pms->count[0] < nbits)
      pms->count[1]++;

   /* Process an initial partial block. */
   if (offset) {
      int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

      memcpy(pms->buf + offset, p, copy);
      if (offset + copy < 64)
         return;
      p += copy;
      left -= copy;
      md5_process(pms, pms->buf);
   }

   /* Process full blocks. */
   for (; left >= 64; p += 64, left -= 64)
      md5_process(pms, p);

   /* Process a final partial block. */
   if (left)
      memcpy(pms->buf, p, left);
}

void md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
   static const md5_byte_t pad[64] = 
   {
      0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
   };
   md5_byte_t data[8];
   int i;

   /* Save the length before padding. */
   for (i = 0; i < 8; ++i)
      data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
   /* Pad to 56 bytes mod 64. */
   md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
   /* Append the length. */
   md5_append(pms, data, 8);
   for (i = 0; i < 16; ++i)
      digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}

//-----------------------------------------------------------------------------

void Crypto::Bin2Hex(const unsigned char* pData, size_t pDataSize, std::string& sOut)
{
   static const char bin2hex[] = { '0', '1', '2', '3',
                                   '4', '5', '6', '7',
                                   '8', '9', 'a', 'b',
                                   'c', 'd', 'e', 'f'};
   sOut.clear();
   sOut.reserve(pDataSize*2);

   unsigned char lo, hi;

   size_t hexlen = pDataSize*2;

   for (size_t i = 0; (i < pDataSize); i++) 
   {
      lo = pData[i] & 0xf;
      hi = pData[i] >> 4;
      sOut.append(1, bin2hex[hi]);
      sOut.append(1, bin2hex[lo]);
   }
   
}

//-----------------------------------------------------------------------------

void Crypto::Hex2Bin(std::string sDataIn, shared_array<unsigned char>& qOut, size_t& nSizeOut)
{
   int hexlen = sDataIn.size();
   int binlen = hexlen/2;

   // there is something wrong with hex-encoded data!
   if (binlen*2 != hexlen)
   {
      assert(false);
      return;
   }

   qOut = shared_array<unsigned char>(new unsigned char[binlen]);
   nSizeOut = (size_t) binlen;


   int i = 0, j = 0;
   unsigned char lo, hi, result, tmp;

   while (i < hexlen && j < binlen) 
   {
      hi = lo = 0;

      tmp = sDataIn[i++];
      if (tmp >= '0' && tmp <= '9')
         hi = tmp - '0';
      else if (tmp >= 'a' && tmp <= 'f')
         hi = 10 + (tmp - 'a');

      tmp = sDataIn[i++];
      if (tmp >= '0' && tmp <= '9')
         lo = tmp - '0';
      else if (tmp >= 'a' && tmp <= 'f')
         lo = 10 + (tmp - 'a');

      result = hi << 4;
      result += lo;
      qOut.get()[j++] = result;
   }
}

//-----------------------------------------------------------------------------


/*
static void Bin2Hex (const unsigned char *bin, int binlen,
		    unsigned char *hex, int hexlen) 
{
    static const char bin2hex[] = { '0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f'
    };

    unsigned char lo, hi;
    int i, pos;
    for (i = 0, pos = 0; (i < binlen && pos < hexlen); i++) {
	lo = bin[i] & 0xf;
	hi = bin[i] >> 4;
	hex[pos++] = bin2hex[hi];
	hex[pos++] = bin2hex[lo];
    }

    hex[pos] = '\0';
}

static int Hex2Bin (const unsigned char *hex, int hexlen,
		    unsigned char *bin, int binlen)
{
    int i = 0, j = 0;
    unsigned char lo, hi, result, tmp;

    while (i < hexlen && j < binlen) 
    {
	   hi = lo = 0;

	   tmp = hex[i++];
	   if (tmp >= '0' && tmp <= '9')
	       hi = tmp - '0';
	   else if (tmp >= 'a' && tmp <= 'f')
	       hi = 10 + (tmp - 'a');

	   tmp = hex[i++];
	   if (tmp >= '0' && tmp <= '9')
	       lo = tmp - '0';
	   else if (tmp >= 'a' && tmp <= 'f')
	       lo = 10 + (tmp - 'a');

	   result = hi << 4;
	   result += lo;
	   bin[j++] = result;
    }

    return j;
}

*/