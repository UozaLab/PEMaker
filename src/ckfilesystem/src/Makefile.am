EXTRA_DIST = ../include/ckfilesystem/const.hh \
			 ../include/ckfilesystem/dvdvideo.hh \
			 ../include/ckfilesystem/eltorito.hh \
			 ../include/ckfilesystem/exception.hh \
			 ../include/ckfilesystem/fileset.hh \
			 ../include/ckfilesystem/filesystem.hh \
			 ../include/ckfilesystem/filesystemhelper.hh \
			 ../include/ckfilesystem/filesystemwriter.hh \
			 ../include/ckfilesystem/filetree.hh \
			 ../include/ckfilesystem/iforeader.hh \
			 ../include/ckfilesystem/iso.hh \
			 ../include/ckfilesystem/isoreader.hh \
			 ../include/ckfilesystem/isowriter.hh \
			 ../include/ckfilesystem/joliet.hh \
			 ../include/ckfilesystem/sectormanager.hh \
			 ../include/ckfilesystem/sectorstream.hh \
			 ../include/ckfilesystem/stringtable.hh \
			 ../include/ckfilesystem/udf.hh \
			 ../include/ckfilesystem/udfwriter.hh \
			 ../include/ckfilesystem/util.hh \
			 ../include/ckfilesystem/isopathtable.hh

AM_CPPFLAGS = -I$(srcdir)/../include -std=c++11 -DUNICODE -D_UNICODE -D_WINDOWS
lib_LTLIBRARIES = libckfilesystem.la
#bin_PROGRAMS = verifier

libckfilesystem_la_SOURCES = dvdvideo.cc eltorito.cc filesystem.cc \
							 filesystemwriter.cc filesystemhelper.cc \
							 filetree.cc iforeader.cc iso.cc \
							 isoreader.cc isowriter.cc joliet.cc \
							 sectormanager.cc sectorstream.cc stringtable.cc \
							 udf.cc udfwriter.cc util.cc \
							 isopathtable.cc

libckfilesystem_la_LDFLAGS = -version-info $(CKFILESYSTEM_VERSION)

#verifier_SOURCES = isoverifier.cc verifier.cc
#verifier_LDADD = libckfilesystem.la
#verifier_LDFLAGS = -ldl -lpthread

library_includedir = $(includedir)/ckfilesystem
library_include_HEADERS = ../include/ckfilesystem/const.hh \
						  ../include/ckfilesystem/dvdvideo.hh \
						  ../include/ckfilesystem/eltorito.hh \
						  ../include/ckfilesystem/exception.hh \
						  ../include/ckfilesystem/fileset.hh \
						  ../include/ckfilesystem/filesystem.hh \
						  ../include/ckfilesystem/filesystemhelper.hh \
						  ../include/ckfilesystem/filesystemwriter.hh \
						  ../include/ckfilesystem/filetree.hh \
						  ../include/ckfilesystem/iforeader.hh \
						  ../include/ckfilesystem/iso.hh \
						  ../include/ckfilesystem/isoreader.hh \
						  ../include/ckfilesystem/isowriter.hh \
						  ../include/ckfilesystem/joliet.hh \
						  ../include/ckfilesystem/sectormanager.hh \
						  ../include/ckfilesystem/sectorstream.hh \
						  ../include/ckfilesystem/stringtable.hh \
						  ../include/ckfilesystem/udf.hh \
						  ../include/ckfilesystem/udfwriter.hh \
						  ../include/ckfilesystem/util.hh \
						  ../include/ckfilesystem/isopathtable.hh