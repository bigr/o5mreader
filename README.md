# o5mreader - o5m parser


## About

O5mreader is a C library that parses OpenStreetMap data in O5M format.

## Installation

	./configure
	make
	sudo make install

## Example

	O5mreader* reader;
	O5mreaderDataset ds;
	O5mreaderIterateRet ret, ret2;
	char *key, *val;
	int64_t nodeId;
	int64_t refId;
	uint8_t type;
	char *role;

	f = fopen("some-file.o5m","rb");
	while( (ret = o5mreader_iterateDataSet(reader, &ds)) == O5MREADER_ITERATE_RET_NEXT ) {
		switch ( ds.type ) {
		// Data set is node
		case O5MREADER_DS_NODE:
			// Could do something with ds.id, ds.lon, ds.lat here, lon and lat are ints in 1E+7 * degree units
			// Node tags iteration, can be omited
			while ( (ret2 = o5mreader_iterateTags(reader,&key,&val)) == O5MREADER_ITERATE_RET_NEXT  ) {
				// Could do something with tag key and val
			}
			break;
		// Data set is way
		case O5MREADER_DS_WAY:
			// Could do something with ds.id
			// Nodes iteration, can be omited
			while ( (ret2 = o5mreader_iterateNds(reader,&nodeId)) == O5MREADER_ITERATE_RET_NEXT  ) {
				// Could do something with nodeId
			}
			// Way taga iteration, can be omited
			while ( (ret2 = o5mreader_iterateTags(reader,&key,&val)) == O5MREADER_ITERATE_RET_NEXT  ) {
				// Could do something with tag key and val
			}
			break;
		// Data set is rel
		case O5MREADER_DS_REL:
			// Could do something with ds.id
			// Refs iteration, can be omited
			while ( (ret2 = o5mreader_iterateRefs(reader,&refId,&type,&role)) == O5MREADER_ITERATE_RET_NEXT  ) {
				// Could do something with refId (way or node or rel id depends on type), type and role
			}
			// Relation tags iteration, can be omited
			while ( (ret2 = o5mreader_iterateTags(reader,&key,&val)) == O5MREADER_ITERATE_RET_NEXT  ) {
				// Could do something with tag key and val
			}
			break;
		}
	}

	fclose(f);

## More

More information about o5m format: <http://wiki.openstreetmap.org/wiki/O5m>
