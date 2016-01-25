### NODLib

**NODLib** is a library and utility (`nodtool`) for traversing, dumping, and authoring 
*GameCube* and *Wii* optical disc images.

#### Library

The primary motivation of NODLib is to supply a *uniform C++11 API* for accessing data
from image files directly. `NOD::DiscBase` provides a common interface for traversing partitions
and individual files. Files may be individually streamed, or the whole partition may be extracted
to the user's filesystem. Raw *ISO* and *WBFS* images are supported read sources. 

```cpp
bool isWii; /* Set by reference next line */
std::unique_ptr<NOD::DiscBase> disc = NOD::OpenDiscFromImage(path, isWii);
if (!disc)
	return FAILURE;

/* Access first data-partition on Wii, or full GameCube disc */
NOD::Partition* dataPart = disc->getDataPartition();
if (!dataPart)
	return FAILURE;

/* One-shot extraction to filesystem */
if (!dataPart->extractToDirectory(outDir, ctx))
	return FAILURE;
	
return SUCCESS;
```

*Image authoring* is always done from the user's filesystem and may be integrated into
a content pipeline using the `NOD::DiscBuilderBase` interface.

```cpp
/* Sample logging lambda for progress feedback */
size_t lastIdx = -1;
auto progFunc = [&](size_t idx, const NOD::SystemString& name, size_t bytes)
{
	if (idx != lastIdx)
	{
		lastIdx = idx;
		/* NOD provides I/O wrappers using wchar_t on Windows; 
		 * _S() conditionally makes string-literals wide */
		NOD::Printf(_S("\n")); 
	}
	if (bytes != -1)
		NOD::Printf(_S("\r%s %" PRISize " B"), name.c_str(), bytes);
	else
		NOD::Printf(_S("\r%s"), name.c_str());
	fflush(stdout);
};

/* Making a GCN image */
NOD::DiscBuilderGCN b(isoOutPath, gameID, gameTitle, dolLoadAddress, progFunc);
ret = b.buildFromDirectory(fsRootDirPath, bootDolPath, apploaderPath);

/* Making a Wii image */
NOD::DiscBuilderWii b(isoOutPath, gameID, gameTitle, dualLayer, progFunc);
ret = b.buildFromDirectory(fsRootDirPath, bootDolPath, apploaderPath, partitionHeadPath);
```

Wii images are fakesigned using a commonly-applied [signing bug](http://wiibrew.org/wiki/Signing_bug).

Additionally, any `*.dol` files added to the disc are patched to bypass the #001 error caused by invalid signature checks.
This allows games with multiple .dols to inter-boot without extensive loader-patching.

#### Tool

The library usage mentioned above is provided by a command-line tool called `nodtool`.

An extract/repack works like so:

```sh
>$ nodtool extract <image-in> [<dir-out>]
>$ cd <dir-out>

# Then one of:
>$ nodtool makegcn <gameid> <game-title> fsroot boot.dol apploader.bin [<image-out>]
>$ nodtool makewiisl <gameid> <game-title> fsroot boot.dol apploader.bin partition_head.bin [<image-out>]
>$ nodtool makewiidl <gameid> <game-title> fsroot boot.dol apploader.bin partition_head.bin [<image-out>]
```
