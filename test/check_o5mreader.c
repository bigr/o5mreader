#include <check.h>
#include <stdlib.h>
#include "../src/o5mreader.h"


START_TEST (check_o5mreader_open) {
	FILE* f;
	O5mreader* reader;
	char str[255];
    
	f = fopen("files/null.o5m","rb");
	if ( !f ) fail ("File 'files/null.o5m' can't be opened.");
	if ( o5mreader_open(&reader,f) ) {
		fail("File 'files/null.o5m' opened without error.");
	}
	else {
		sprintf(str,
			"Failed with wrong error: '%s' while opening file 'files/null.o5m'.",
			o5mreader_strerror(reader->errCode)
		);
		fail_unless(
			O5MREADER_ERR_CODE_UNEXPECTED_END_OF_FILE == reader->errCode,
			str
		);
	}
	o5mreader_close(reader);
	fclose(f);
		
	f = fopen("files/invalid.o5m","rb");
	if ( !f ) fail ("File 'files/invalid.o5m' can't be opened.");
	if ( o5mreader_open(&reader,f) ) {
		fail("File 'files/invalid.o5m' opened without error.");
	}
	else {
		sprintf(str,
			"Failed with wrong error: '%s' while opening file 'files/invalid.o5m'.",
			o5mreader_strerror(reader->errCode)
		);
		fail_unless(
			O5MREADER_ERR_CODE_FILE_HAS_WRONG_START == reader->errCode,
			str
		);
	}
	o5mreader_close(reader);	
	fclose(f);
	    
	f = fopen("files/empty.o5m","rb");
	if ( !f ) fail ("File 'files/empty.o5m' can't be opened.");
	if ( !o5mreader_open(&reader,f) ) {
		sprintf(str,
			"Failed with error: '%s' while opening file 'files/empty.o5m'.",
			o5mreader_strerror(reader->errCode)
		);
		fail (str);
		fclose(f);
	}      
	o5mreader_close(reader);
	fclose(f);
}
END_TEST

START_TEST (check_o5mreader_iterateDataSet) {
	const uint64_t IDS[4] = {125799LL,125797LL,43LL,16LL};
	const int32_t lons[4] = { 87867843, 87867845,-35,-60};
	const int32_t lats[4] = {530749606,530749602, 52, 63};	
	
	FILE* f;
	O5mreader* reader;
	O5mreaderDataset ds;
	O5mreaderIterateRet ret;
	char str[255];
	int i;
	
	f = fopen("files/ds.o5m","rb");
	if ( !f ) fail ("File 'files/ds.o5m' can't be opened.");
	if ( !o5mreader_open(&reader,f) ) {
		sprintf(str,
			"Failed with error: '%s' while opening file 'files/ds.o5m'.",
			o5mreader_strerror(reader->errCode)
		);
		fail (o5mreader_strerror(reader->errCode));
		fclose(f);
	}
	else {
		i = 0;
		while( (ret = o5mreader_iterateDataSet(reader, &ds)) == O5MREADER_ITERATE_RET_NEXT ) {
			++i;
			sprintf(str,
				"Node type expected but have: '%d' like (data set number: '%d', file: 'files/ds.o5m').",
				ds.type, i
			);
			fail_unless(ds.type == O5MREADER_DS_NODE, str);
			
			sprintf(str,
				"Expected node id '%llu', but got: '%llu' (data set number: '%d', file: 'files/ds.o5m').",
				IDS[i-1], ds.id, i
			);
			fail_unless(ds.id == IDS[i-1], str);
			
			sprintf(str,
				"Expected lon '%d', but got: '%d' (data set number: '%d', file: 'files/ds.o5m').",
				lons[i-1], ds.lon, i
			);
			fail_unless(ds.lon == lons[i-1], str);
			
			sprintf(str,
				"Expected lat '%d', but got: '%d' (data set number: '%d', file: 'files/ds.o5m').",
				lats[i-1], ds.lat, i
			);
			fail_unless(ds.lat == lats[i-1], str);
		}
		
		if ( O5MREADER_ITERATE_RET_ERR == ret ) {
			sprintf(str,
				"Iteration of 'files/ds.o5m' failed. %s %s",
				o5mreader_strerror(reader->errCode),
				reader->errMsg ? reader->errMsg : ""
			);
			fail(str);
		}
		
		sprintf(str,
			"Iterated wrong number of data sets (%d <> 5) during 'files/ds.o5m' iteration.",
			i
		);
		fail_unless(4 == i, str);
	}
	o5mreader_close(reader);
	fclose(f);
	
	
	
	
	
}
END_TEST

START_TEST (check_o5mreader_iterateTags) {
	const uint64_t IDS[9] = {1LL,5LL,8LL,9LL,500LL,800LL,9000LL,10000LL,11000LL};
	const int32_t lons[4] = {150000000,160000000,100000000,130000000};
	const int32_t lats[4] = {500000000,600000000,480000000,450000000};	
	const char* tags[9][3][2] = {
		{
			{"tag1","value1"},
			{"tag2","val2"},
			0
		},
		{
			{"tag3","val3"},
			{"tagtag4","val4"},
			{"tag2","val2"}
		},
		{0,0,0},
		{
			{"tag1","value1"},
			{"tag2","val2"},
			{"tagtag4","val4"}				
		},
		{
			{"tag2","val2"},
			{"tag5","val7"},
			0
		},
		{
			{"tag6","value7"},
			0,
			0
		},
		{
			{"tag1","val1"},
			{"tag7","val7"},
			0
		},
		{0,0,0},
		{0,0,0}
	};

    const int tags_count[9] = {2,3,0,3,2,1,2,0,0};
    
    const uint64_t nds[2][3] = {
		{1,5},
		{1,8,9}
	};
	
	const uint64_t refs[3][4][3] = {
		{
			{O5MREADER_DS_NODE,"r1",5},
			{O5MREADER_DS_NODE,"role2",9},
			{O5MREADER_DS_WAY,"",500},
			{O5MREADER_DS_WAY,"",800}
		},
		{
			{O5MREADER_DS_REL,"rl3",9000},
			{0,0,0},
			{0,0,0},
			{0,0,0}
		},
		{
			{O5MREADER_DS_REL,"rl3",9000},
			{0,0,0},
			{0,0,0},
			{0,0,0}
		}
	};
    
	FILE* f;
	O5mreader* reader;
	O5mreaderDataset ds;
	O5mreaderIterateRet ret, ret2;
	char str[255];
	int i,j;
	char *key, *val;
	int64_t nodeId;
	int64_t refId;
	uint8_t type;
	char *role;
	
	f = fopen("files/test1.o5m","rb");
	if ( !f ) fail ("File 'files/test1.o5m' can't be opened.");
	if ( !o5mreader_open(&reader,f) ) {
		sprintf(str,
			"Failed with error: '%s' while opening file 'files/test1.o5m'.",
			o5mreader_strerror(reader->errCode)
		);
		fail (o5mreader_strerror(reader->errCode));
		fclose(f);
	}
	else {
		i = 0;
		while( (ret = o5mreader_iterateDataSet(reader, &ds)) == O5MREADER_ITERATE_RET_NEXT ) {			
			++i;
			if ( i <= 4 ) {
				sprintf(str,
					"Node type expected but have: '%d' like (data set number: '%d', file: 'files/test1.o5m').",
					ds.type, i
				);
				fail_unless(ds.type == O5MREADER_DS_NODE, str);
			}
			else if ( i <= 6 ) {
				sprintf(str,
					"Way type expected but have: '%d' like (data set number: '%d', file: 'files/test1.o5m').",
					ds.type, i
				);
				fail_unless(ds.type == O5MREADER_DS_WAY, str);
			}
			else {
				sprintf(str,
					"Relation type expected but have: '%d' like (data set number: '%d', file: 'files/test1.o5m').",
					ds.type, i
				);
				fail_unless(ds.type == O5MREADER_DS_REL, str);
			}
			
			sprintf(str,
				"Expected node id '%llu', but got: '%llu' (data set number: '%d', file: 'files/test1.o5m').",
				IDS[i-1], ds.id, i
			);
			fail_unless(ds.id == IDS[i-1], str);
			
			if ( i <= 4 ) {
				sprintf(str,
					"Expected lon '%d', but got: '%d' (data set number: '%d', file: 'files/test1.o5m').",
					lons[i-1], ds.lon, i
				);
				fail_unless(ds.lon == lons[i-1], str);
				
				sprintf(str,
					"Expected lat '%d', but got: '%d' (data set number: '%d', file: 'files/test1.o5m').",
					lats[i-1], ds.lat, i
				);
				fail_unless(ds.lat == lats[i-1], str);
			}
			else if ( i <= 6 ) {
				j = 0;
				while ( (ret2 = o5mreader_iterateNds(reader,&nodeId)) == O5MREADER_ITERATE_RET_NEXT  ) {
					++j;
					sprintf(str,
						"Expected node id '%lld', but got: '%lld' (data set number: '%d', node number '%d', file: 'files/test1.o5m').",
						nds[i-5][j-1], nodeId, i, j
					);
					fail_unless( nodeId == nds[i-5][j-1], str);
				}
			}
			else {
				j = 0;
				while ( (ret2 = o5mreader_iterateRefs(reader,&refId,&type,&role)) == O5MREADER_ITERATE_RET_NEXT  ) {
					++j;
					sprintf(str,
						"Expected ref id '%lld', but got: '%lld' (data set number: '%d', node number '%d', file: 'files/test1.o5m').",
						refs[i-7][j-1][2], refId, i, j
					);
					fail_unless( refId == refs[i-7][j-1][2], str);
					sprintf(str,
						"Expected type '%lld', but got: '%lld' (data set number: '%d', node number '%d', file: 'files/test1.o5m').",
						refs[i-7][j-1][0], type, i, j
					);
					fail_unless( type == refs[i-7][j-1][0], str);
					sprintf(str,
						"Expected role '%s', but got: '%s' (data set number: '%d', node number '%d', file: 'files/test1.o5m').",
						refs[i-7][j-1][1], role, i, j
					);
					fail_unless( 0 == strcmp(role,refs[i-7][j-1][1]), str);
				}
			}
			
			
			j = 0;			
			while ( (ret2 = o5mreader_iterateTags(reader,&key,&val)) == O5MREADER_ITERATE_RET_NEXT  ) {
				++j;
				sprintf(str,
					"Expected tag key '%s', but got: '%s' (data set number: '%d', tag number '%d', file: 'files/test1.o5m').",
					tags[i-1][j-1][0], key, i, j
				);
				fail_unless( 0 == strcmp(key,tags[i-1][j-1][0]), str);
				sprintf(str,
					"Expected tag value '%s', but got: '%s' (data set number: '%d', tag number '%d', file: 'files/test1.o5m').",
					tags[i-1][j-1][1], val, i, j
				);
				fail_unless( 0 == strcmp(val,tags[i-1][j-1][1]), str);
			}
			
			if ( O5MREADER_ITERATE_RET_ERR == ret2 ) {
				sprintf(str,
					"Iteration of tags (data set number: '%d', file: 'files/test1.o5m') failed. %s %s",
					i,
					o5mreader_strerror(reader->errCode),
					reader->errMsg ? reader->errMsg : ""
				);
				fail(str);
			}
			
			sprintf(str,
				"Iterated wrong number of tags (%d <> %d) (data set number: '%d', file: 'files/test1.o5m') iteration.",
				j,tags_count[i-1],i
			);
			fail_unless(tags_count[i-1] == j, str);			
		}
		
		if ( O5MREADER_ITERATE_RET_ERR == ret ) {
			sprintf(str,
				"Iteration of 'files/test1.o5m' failed. %s %s",
				o5mreader_strerror(reader->errCode),
				reader->errMsg ? reader->errMsg : ""
			);
			fail(str);
		}
		
		sprintf(str,
			"Iterated wrong number of data sets (%d <> 9) during 'files/test1.o5m' iteration.",
			i
		);
		fail_unless(9 == i, str);
	}
	o5mreader_close(reader);
	fclose(f);
}
END_TEST

START_TEST (check_o5mreader_iterateNds) {
  /* unit test code */
}
END_TEST

START_TEST (check_o5mreader_iterateRefs) {
  /* unit test code */
}
END_TEST



Suite *o5mreader_suite (void) {
	Suite *s = suite_create ("O5mreader");
	TCase *tc_core = tcase_create ("Core");	
	tcase_add_test (tc_core, check_o5mreader_open);	
	tcase_add_test (tc_core, check_o5mreader_iterateDataSet);
	tcase_add_test (tc_core, check_o5mreader_iterateTags);
	tcase_add_test (tc_core, check_o5mreader_iterateNds);
	tcase_add_test (tc_core, check_o5mreader_iterateRefs);	
	suite_add_tcase (s, tc_core);

	return s;
}

main (void) {

	int number_failed;
	Suite *s = o5mreader_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }
