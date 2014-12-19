from ctypes import *
from copy import *

class Dataset(Structure):
	_fields_ = [
		("type", c_uint8),
		("id", c_uint64),
		("version", c_uint32),
		("isEmpty", c_uint8),
        ("lon", c_int32),
        ("lat", c_int32)
    ]              


class _O5mreader(Structure):
	_fields_ = [
		("errCode", c_int),
		("errMsg", c_char_p),
        ("f", c_int),
        ("offset", c_uint64),
        ("offsetNd", c_uint64),
        ("offsetRf", c_uint64),
        ("current", c_uint64),
        ("tagPair", c_char_p),
        ("nodeId", c_uint64),
        ("wayId", c_uint64),
        ("wayNodeId", c_uint64),
        ("relId", c_uint64),
        ("nodeRefId", c_uint64),
        ("wayRefId", c_uint64),
        ("relRefId", c_uint64),
        ("lon", c_int32),
        ("lat", c_int32),
        ("canIterateTags", c_uint8),
        ("canIterateNds", c_uint8),
        ("canIterateRefs", c_uint8),
        ("strPairTable",POINTER(c_char_p))
        
    ]

class O5mreaderException(Exception):
	pass

class Element:
	def __init__(self,reader,id,version,isEmpty,tags=True):
		self._reader = reader
		self.id = id
		self.version = version
		self.isEmpty = isEmpty
		self.type = 0
		if not self.isEmpty and tags:
			self.tags = self._tags()	
	
	def _tags(self):
		key = c_char_p()
		val = c_char_p()
		
		ret = {}
		while 1 == self._reader.libo5mreader.o5mreader_iterateTags(self._reader._reader,byref(key),byref(val)):			
			ret[unicode(key.value,'utf-8',errors='replace')] = unicode(val.value,'utf-8',errors='replace')
			
		if self._reader._reader.contents.errCode != 0:			
			raise O5mreaderException(string_at(self._reader.libo5mreader.o5mreader_strerror(self._reader._reader.contents.errCode)));
			
		return ret
			
		

class Node(Element):
	def __init__(self,reader,id,version,isEmpty,lon,lat,tags=True):
		Element.__init__(self,reader,id,version,isEmpty,tags=True)
		if self.isEmpty:
			self.lon = None
			self.lat = None
		else:
			self.lon = lon
			self.lat = lat			
		self.type = O5mreader.NODE
		
class Way(Element):
	def __init__(self,reader,id,version,isEmpty,nds=True,tags=True):
		Element.__init__(self,reader,id,version,isEmpty,tags=False)
		if not self.isEmpty:
			if nds:
				self.nds = self._nds()		
			if tags:
				self.tags = self._tags()
					
		self.type = O5mreader.WAY
		
	def _nds(self):
		nodeId = c_uint64()
				
		ret = []
		while 1 == self._reader.libo5mreader.o5mreader_iterateNds(self._reader._reader,byref(nodeId)):
			ret.append(nodeId.value)
		
		if self._reader._reader.contents.errCode != 0:			
			raise O5mreaderException(string_at(self._reader.libo5mreader.o5mreader_strerror(self._reader._reader.contents.errCode)));
			
		return ret
	
class Relation(Element):
	def __init__(self,reader,id,version,isEmpty,refs=True,tags=True):
		Element.__init__(self,reader,id,version,isEmpty,tags=False)
		if not self.isEmpty:
			if refs:
				self.refs = self._refs()
			if tags:
				self.tags = self._tags()
		self.type = O5mreader.RELATION
		
	def _refs(self):
		refId = c_uint64()
		type = c_uint8()
		role = c_char_p()
				
		ret = []
		while 1 == self._reader.libo5mreader.o5mreader_iterateRefs(self._reader._reader,byref(refId),byref(type),byref(role)):
			ret.append((refId.value,type.value,copy(unicode(role.value,'utf-8'))))
		
		if self._reader._reader.contents.errCode != 0:			
			raise O5mreaderException(string_at(self._reader.libo5mreader.o5mreader_strerror(self._reader._reader.contents.errCode)));
			
		return ret
	

class O5mreader:
	libo5mreader = CDLL("libo5mreader.so")
	libc = CDLL("libc.so.6")
	
	NODE = 0x10
	WAY = 0x11
	RELATION = 0x12
	
	def __init__(self,f):
		self._f = f
		fd = self.libc.fdopen(self._f.fileno(),"rb")
		self._reader = pointer(_O5mreader())
		self.libo5mreader.o5mreader_open(byref(self._reader),fd)
		

	def __del__(self):		
		self.libo5mreader.o5mreader_close(self._reader)
		
		
	def __iter__(self):	
		ds = Dataset()	
		while 1 == self.libo5mreader.o5mreader_iterateDataSet(self._reader,byref(ds)):
			if ds.type == self.NODE:
				yield Node(self,ds.id,ds.version,ds.isEmpty,ds.lon,ds.lat)
			elif ds.type == self.WAY:
				yield Way(self,ds.id,ds.version,ds.isEmpty)
			elif ds.type == self.RELATION:
				yield Relation(self,ds.id,ds.version,ds.isEmpty)
				
		if self._reader.contents.errCode != 0:			
			raise O5mreaderException(string_at(self.libo5mreader.o5mreader_strerror(self._reader.contents.errCode)));
		
