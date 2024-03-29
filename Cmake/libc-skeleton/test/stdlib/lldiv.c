/*
 * Copyright © 2017 Embedded Artistry LLC.
 * License: MIT. See LICENSE file for details.
 */

#include "stdlib_tests.h"
#include <stdlib.h>

// Cmocka needs these
// clang-format off
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
// clang-format on

static struct
{
	uint64_t x, y, div, mod;
} t[] = {
	{0x8ddb1a43e77c4031ull, 0x5950e8c33d34979eull, 0x1ull, 0x348a3180aa47a893ull},
	{0x723f4114006c08c7ull, 0x817de530db2b43fcull, 0x0ull, 0x723f4114006c08c7ull},
	{0x47811fa5f00f74dull, 0x3d98e7d3fcd5d5c5ull, 0x0ull, 0x47811fa5f00f74dull},
	{0x51ffcc7cdc989d43ull, 0x36be8bd6746b70e4ull, 0x1ull, 0x1b4140a6682d2c5full},
	{0x57bf9128512fe829ull, 0x197b3858155d498dull, 0x3ull, 0xb4de82011180b82ull},
	{0x89fc1c5968fa817full, 0xdcea797734c7115ull, 0x9ull, 0xdb838065b4a87c2ull},
	{0x4ed5264cf7092ec5ull, 0xde40d1e15ef3e74ull, 0x5ull, 0x960e4b6895cf681ull},
	{0xffd86b253d97317bull, 0x13f9ff2d24b6d6f4ull, 0xcull, 0x1020750785051e0bull},
	{0x8771fa2da656a721ull, 0x9210fe654c59bfcull, 0xeull, 0x7a31b9503881f59ull},
	{0xb5961d12bcd3e937ull, 0xbdb5a33662f547aull, 0xfull, 0x3bbd40fc00df611ull},
	{0x93c79eecdac7ed3dull, 0x6f267c57ea2b7b5ull, 0x15ull, 0x1e51bb9776edb64ull},
	{0x6b93ffce49f1a4b3ull, 0x3583d1f9702ee03ull, 0x20ull, 0x8c5bdb6993e453ull},
	{0x138aefcc98ce5d19ull, 0x117002fa7600b11ull, 0x11ull, 0x103eca27b6da0f8ull},
	{0xb3da641cef491fefull, 0x357615f638334b8ull, 0x35ull, 0x2c33b5d551f35d7ull},
	{0x71c4b06e463912b5ull, 0x1c286ad9e8f5229ull, 0x40ull, 0x1230506a2648875ull},
	{0x97d4cf7df046d6ebull, 0x1e9412f5c77b2b8ull, 0x4full, 0xd9b1e06756b023ull},
	{0x1428f04bd490ea11ull, 0x9d97f29a897c93ull, 0x20ull, 0x75f1f8836157b1ull},
	{0x35256c76832705a7ull, 0xa962f1a447dcd7ull, 0x50ull, 0x3680f32cb20277ull},
	{0x2969e82bd9347f2dull, 0x723d68574d4156ull, 0x5cull, 0x5bd6ac79710445ull},
	{0x9061a12aae71a823ull, 0x4186d8a1a66175ull, 0x234ull, 0x48be68be2f25full},
	{0x695b8d33ef342e09ull, 0x3ed1fe1a998fe3ull, 0x1adull, 0x15a6615bde0ea2ull},
	{0x46b4dd1e06367a5full, 0xa04e70622e4e8ull, 0x70eull, 0x64750bc0b9dafull},
	{0xd68b05ba7eee12a5ull, 0x72ab3fb682444ull, 0x1defull, 0x3c437fc988329ull},
	{0x1e59cc2ac508f85bull, 0xeb15ae6d4d7f9ull, 0x210ull, 0xc00aeae0b86cbull},
	{0x296f8d2c76a0901ull, 0xf65628b31b01ull, 0x2b0ull, 0xf14566117651ull},
	{0x7036f5ad7cbc5e17ull, 0xa09d3bfcf72cfull, 0xb2dull, 0x72236db564ab4ull},
	{0x915d6883c575ad1dull, 0x3a38d68d3a38eull, 0x27f2ull, 0x241de6f7a6ee1ull},
	{0x845ba74f5adfa793ull, 0x2f6950e58d00bull, 0x2caaull, 0x249dc90239c45ull},
	{0xb910d16c54805af9ull, 0x1fc2ca5c99a7aull, 0x5d3aull, 0x1771487b50955ull},
	{0x27a2e280bcf990cfull, 0x389aa0c0b0cc0ull, 0xb34ull, 0x9d71d12eb9cfull},
	{0x1e032f04a5372e95ull, 0x63c2a1d58710ull, 0x4d04ull, 0x154ce4414255ull},
	{0x3a1a5659908495cbull, 0x279dcd85418aull, 0x17775ull, 0x132c6f9c7bb9ull},
	{0xd769a376e5e103f1ull, 0xadacb670e0c7ull, 0x13d85ull, 0x8ad256e5d18eull},
	{0x269f4f4baaaf287ull, 0x1aed2ad9daf0ull, 0x16f3ull, 0x426550f80b7ull},
	{0x6700daeeb87a770dull, 0xeca7ab1aa93ull, 0x6f6c5ull, 0x70d9466f1eeull},
	{0xd0201f3783c2a303ull, 0x3a0c01aa3e6aull, 0x395e1ull, 0x18b33b9015d9ull},
	{0xca3f2e00d291e3e9ull, 0xbe0e048cd94ull, 0x1106c2ull, 0x37f7fc0a1c1ull},
	{0xec4d240dc289633full, 0x4f8aadb7483ull, 0x2f8855ull, 0x46e0db91bc0ull},
	{0xd7967b29e2e36685ull, 0xe61d902db27ull, 0xefd69ull, 0x36811fff886ull},
	{0xe3ecd4374320af3bull, 0x4edd0edd0a0ull, 0x2e3defull, 0x4ad0da4c9dbull},
	{0x7a08fe1d98b4dae1ull, 0x6bced9c0c15ull, 0x121c89ull, 0x40c856617a4ull},
	{0x34435992a5c9c2f7ull, 0x4f4a94c109full, 0xa8bc9ull, 0x94c5d46120ull},
	{0x6fd0027468f1dcfdull, 0x597186b0153ull, 0x140060ull, 0x16f26555dddull},
	{0x4fe37c1db1619a73ull, 0x47a0c30bd15ull, 0x11d861ull, 0x5964fb3d7eull},
	{0x77aa77f86d07c8d9ull, 0x3a39cf03d65ull, 0x20e21cull, 0x37f7fede7cdull},
	{0xc072e76ad59cf1afull, 0x3a786701dull, 0x34a98c59ull, 0x22b6b1b9aull},
	{0xfb8e8f1f7781ba75ull, 0xe8ca427d3eull, 0x114a34dull, 0xa344eb94cfull},
	{0x160e34cf590444abull, 0xe2388f12feull, 0x18f574ull, 0xc303329393ull},
	{0x2509ddea3a648dd1ull, 0xec762d81bcull, 0x281955ull, 0xc0463d1e65ull},
	{0xc9ba10cd6eafcf67ull, 0x96a51d06f7ull, 0x156ce72ull, 0x133e2df369ull},
	{0x1dd4fe261b4adeedull, 0x2736e25406ull, 0xc2bfefull, 0x1354c1f353ull},
	{0x480258f92fc38de3ull, 0x2599b52bb0ull, 0x1ea450cull, 0x2879f11a3ull},
	{0x5a3257b1114109c9ull, 0x2978f9f1aaull, 0x22cc30aull, 0x1317311b25ull},
	{0xf4eeda8f34ab3c1full, 0x1aa70450d9ull, 0x9309d64ull, 0x1187b6925bull},
	{0x3c2c319ca8612a65ull, 0x73fc01eceull, 0x84d0088ull, 0x3165accf5ull},
	{0x4f6034e74a16561bull, 0x1f29d53707ull, 0x28c0daaull, 0xd88e07075ull},
	{0x206665a7072f1cc1ull, 0xda87e7ceaull, 0x25f48c1ull, 0xd3ddb2057ull},
	{0x100c559d7db417d7ull, 0xb907ebbc2ull, 0x1634188ull, 0xa2eae16c7ull},
	{0x64c5f83691b47cddull, 0x5aced6ebbull, 0x11c17fb7ull, 0x344109030ull},
	{0x32a812777eaf7d53ull, 0x1cb63fe4full, 0x1c3a9675ull, 0xb113f938ull},
	{0x67478d96865ca6b9ull, 0x142fa03aull, 0x51dcb463dull, 0x11359ce7ull},
	{0x71024e740deb428full, 0x142d3885ull, 0x599d9edd5ull, 0x13b1ae6ull},
	{0x52c78160b090b655ull, 0xd02101c6ull, 0x65d1b205ull, 0x1c0a0177ull},
	{0x16917d5f9fde38bull, 0xfb1566c7ull, 0x17029e0ull, 0x1bbe166bull},
	{0xa6ee688a0d1387b1ull, 0x22c4d384ull, 0x4cd19afcfull, 0x77143f5ull},
	{0x74babc1be2ed9c47ull, 0x22eda9a6ull, 0x3578b1967ull, 0x189b247dull},
	{0x7c5cbf2dfc1db6cdull, 0x5f09c060ull, 0x14efd44d4ull, 0x5210e74dull},
	{0x7c046071c1ac68c3ull, 0x3696c8e6ull, 0x24596d86bull, 0x26060a1ull},
	{0x84728ab55d399fa9ull, 0x267d7771ull, 0x370ea7405ull, 0x255d1674ull},
	{0x99f57100ef5404ffull, 0x10c0df86ull, 0x9308fef0dull, 0x9009131ull},
	{0x3f4c0514b0df5e45ull, 0xf2c3810ull, 0x42bf84d39ull, 0x3aa12b5ull},
	{0xd01edb572821ecfbull, 0x2a443aeull, 0x4ec8b88639ull, 0x111c73dull},
	{0xeecb08561bd0cea1ull, 0xbeca231ull, 0x140692508bull, 0x9b36e06ull},
	{0x8c856facc6335cb7ull, 0x398eab4ull, 0x271008c7a5ull, 0x922ab3ull},
	{0x23fb9839e8358cbdull, 0x24deb54ull, 0xf9d714151ull, 0xb9c329ull},
	{0x2005d5de30015033ull, 0x47c06dbull, 0x7240bccbaull, 0x104d115ull},
	{0x67d59c29e076f499ull, 0x179f009ull, 0x465554ac22ull, 0x10b0767ull},
	{0x32d2dd34369c836full, 0x13d3fbfull, 0x2902f2fb54ull, 0x7553c3ull},
	{0x3960c3c99fdc2235ull, 0x1b808baull, 0x21618743cdull, 0x11e7743ull},
	{0x343bad5adfa9726bull, 0xeef444ull, 0x37f58c51a6ull, 0x3d8a53ull},
	{0x7a4aadd7b4e5f191ull, 0x129c9ull, 0x6921bb5a2a53ull, 0x6b66ull},
	{0x9eb7dae5d71c5927ull, 0x31d7f5ull, 0x32f2ff2c6d5ull, 0x22c4eull},
	{0x1b285999316afeadull, 0x115477ull, 0x1912cf6611eull, 0x801bbull},
	{0x917aee3d84b533a3ull, 0x71d26full, 0x1473408589aull, 0x6e74ddull},
	{0x18e6a86b0473a589ull, 0x50a12ull, 0x4f0fabc67d4ull, 0x210a1ull},
	{0xf22c9887813bbddfull, 0x5b17aull, 0x2a897505c07bull, 0x1f841ull},
	{0xef7a551239d60225ull, 0x7fb5aull, 0x1e00b98e188bull, 0x41847ull},
	{0xffd2ad0e77b73dbull, 0x146f14ull, 0xc8500600a3ull, 0xba1full},
	{0x76743abdfb91f081ull, 0xd5888ull, 0x8e0303c479cull, 0x245a1ull},
	{0xc2eeb030bcff9197ull, 0x7a4e8ull, 0x198034e02c37ull, 0x343bfull},
	{0x63cc9c23f0ed0c9dull, 0x6c1e5ull, 0xec4d5841041ull, 0x38178ull},
	{0x7ad70f846e8f1313ull, 0x7fdf5ull, 0xf5ecec69bc9ull, 0x756b6ull},
	{0x60de3d71574eb279ull, 0x6ea3ull, 0xe02421997a61ull, 0x18b6ull},
	{0xd27054901c68b44full, 0x2dd0full, 0x497d639c8f46ull, 0xe135ull},
	{0xbcf297b8f0dbfe15ull, 0xcf17ull, 0xe992af0ca1abull, 0x32b8ull},
	{0x96c3ae70323ef14bull, 0xbbdcull, 0xcd7329b68d81ull, 0x1b6full},
	{0xdc1a13cfa4d3cb71ull, 0xdb16ull, 0x1012fe5ed296full, 0x46e7ull},
	{0xa1d40a2986f40607ull, 0x8067ull, 0x142a473fdb7beull, 0x1895ull},
	{0x227f92ef6daab68dull, 0x15ecull, 0x192dda5d5ed25ull, 0xf71ull},
	{0xc0a4a7810015ee83ull, 0x6064ull, 0x1ffa220762fc8ull, 0x4463ull},
	{0xd38b6eb9f0e71b69ull, 0x1909ull, 0x8732ce2cc77f4ull, 0xfd5ull},
	{0x2e588bdb751a66bfull, 0x229cull, 0x156d025c70d97ull, 0x10bbull},
	{0xd90f7e11dcbd1605ull, 0x760ull, 0x1d6e934381ba2eull, 0x2c5ull},
	{0x60ab67a4e5aeabbull, 0x1bf7ull, 0x374f26f3e3edull, 0x210ull},
	{0x224f627be76a8261ull, 0x4f4ull, 0x6ed4d3882b567ull, 0x35ull},
	{0x300d1ab91bd0b677ull, 0xe9cull, 0x34a002fb76e63ull, 0x823ull},
	{0x2a63d80e0c52fc7dull, 0x32ull, 0xd90970ebc4383full, 0x2full},
	{0xb0e94bbc1f90c5f3ull, 0x3b3ull, 0x2fd2ef70381c52ull, 0x29dull},
	{0x2b5dc22562dbe059ull, 0x30aull, 0xe45055015fff5ull, 0x1c7ull},
	{0x4a7fd1078807d52full, 0x18dull, 0x300a32f60677d4ull, 0x16bull},
	{0x41a01ee8ab0849f5ull, 0x13cull, 0x352a3971f57e9dull, 0x29ull},
	{0x95a7287ad5f6602bull, 0x1d0ull, 0x529130d1034a23ull, 0xbbull},
	{0x723bacc76bd51551ull, 0x16ull, 0x53142091089af83ull, 0xfull},
	{0x81c49febaa2ca2e7ull, 0xcdull, 0xa20d44956f5bf4ull, 0x83ull},
	{0x11593d6b3f54de6dull, 0x63ull, 0x2cdc6b1a7f9078ull, 0x5ull},
	{0x756c82d6f7069963ull, 0x5cull, 0x146bea3ba565525ull, 0x17ull},
	{0xda882ab2a88c0149ull, 0x1bull, 0x8180194d6d5c728ull, 0x11ull},
	{0xbb03671751a7ff9full, 0x20ull, 0x5d81b38ba8d3ffcull, 0x1full},
	{0x6884fa0a8f0c99e5ull, 0x12ull, 0x5ce7fab40d6088cull, 0xdull},
	{0x5052a2953c528441ull, 0x7ull, 0xb7984f0bf79809bull, 0x4ull},
	{0x58dd1583185ecb57ull, 0x9ull, 0x9dfad0e90ee1697ull, 0x8ull},
	{0xaa6870c376df5c5dull, 0x3ull, 0x38cd7aebd24a741full, 0x0ull},
	{0x4b21d01617167e39ull, 0x2ull, 0x2590e80b0b8b3f1cull, 0x1ull},

};

#pragma mark - Private Functions -

static void lldiv_test(void** state)
{
	uint64_t x, y, div, mod;
	size_t i;

	for(i = 0; i < (sizeof(t) / sizeof(*t)); i++)
	{
		x = t[i].x;
		y = t[i].y;
		div = x / y;
		mod = x % y;
		assert_true(div == t[i].div);
		assert_true(mod == t[i].mod);
	}
}

#pragma mark - Public Functions -

int lldiv_tests(void)
{
	const struct CMUnitTest lldiv_tests[] = {cmocka_unit_test(lldiv_test)};

	return cmocka_run_group_tests(lldiv_tests, NULL, NULL);
}
