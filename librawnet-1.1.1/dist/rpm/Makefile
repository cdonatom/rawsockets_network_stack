RPMBUILD_ROOT=~/rpmbuild
RPMBUILD_SOURCES=$(RPMBUILD_ROOT)/SOURCES
RPMBUILD_SPECS=$(RPMBUILD_ROOT)/SPECS
RPMBUILD_BUILD=$(RPMBUILD_ROOT)/BUILD
RPMBUILD_RPMS=$(RPMBUILD_ROOT)/RPMS
RPMBUILD_RPMS_i386=$(RPMBUILD_RPMS)/i386
RPMBUILD_RPMS_x86_64=$(RPMBUILD_RPMS)/x86_64

VERSION=1.1.1
RPM_RELEASE=1

.PHONY: clean i386 x86_64 all

all: i386 x86_64

clean:
	rm -f  $(RPMBUILD_SOURCES)/*
	rm -f  $(RPMBUILD_SPECS)/*
	rm -rf $(RPMBUILD_BUILD)/*
	rm -f  $(RPMBUILD_RPMS_i386)/*
	rm -f  $(RPMBUILD_RPMS_x86_64)/*
	rm -f  *.rpm

i386: librawnet-devel-$(VERSION)-$(RPM_RELEASE).rpm

$(RPMBUILD_SPECS)/%.spec: %.spec
	cp -f $< $@

$(RPMBUILD_SOURCES)/%.patch: %.patch
	cp -f $< $@

$(RPMBUILD_SOURCES)/librawnet-$(VERSION).tar.gz:
	make -C ../../ clean
	ln -s librawnet-$(VERSION)/ ../../../librawnet-devel-$(VERSION)
	tar cvzh -C ../../../ -f $@ librawnet-devel-$(VERSION)/
	rm -f ../../../librawnet-devel-$(VERSION)

$(RPMBUILD_RPMS_i386)/librawnet-devel-$(VERSION)-$(RPM_RELEASE).i386.rpm: \
			$(RPMBUILD_SOURCES)/librawnet-$(VERSION).tar.gz \
			$(RPMBUILD_SOURCES)/rawnetcc_c.patch \
			$(RPMBUILD_SPECS)/librawnet-devel-$(VERSION)-$(RPM_RELEASE).spec
	setarch i386 rpmbuild -ba --target=i386 $(RPMBUILD_SPECS)/librawnet-devel-$(VERSION)-$(RPM_RELEASE).spec

librawnet-devel-$(VERSION)-$(RPM_RELEASE).rpm: $(RPMBUILD_RPMS_i386)/librawnet-devel-$(VERSION)-$(RPM_RELEASE).i386.rpm
	cp $< $@


x86_64: lib64rawnet-devel-$(VERSION)-$(RPM_RELEASE).rpm	    

$(RPMBUILD_SOURCES)/lib64rawnet-$(VERSION).tar.gz:
	make -C ../../ clean
	ln -s librawnet-$(VERSION)/ ../../../lib64rawnet-devel-$(VERSION)
	tar cvzh -C ../../../ -f $@ lib64rawnet-devel-$(VERSION)/
	rm -f ../../../lib64rawnet-devel-$(VERSION)

$(RPMBUILD_RPMS_x86_64)/lib64rawnet-devel-$(VERSION)-$(RPM_RELEASE).x86_64.rpm: \
			$(RPMBUILD_SOURCES)/lib64rawnet-$(VERSION).tar.gz \
			$(RPMBUILD_SOURCES)/rawnetcc_c.patch \
			$(RPMBUILD_SPECS)/lib64rawnet-devel-$(VERSION)-$(RPM_RELEASE).spec
	rpmbuild -ba --target=x86_64 $(RPMBUILD_SPECS)/lib64rawnet-devel-$(VERSION)-$(RPM_RELEASE).spec

lib64rawnet-devel-$(VERSION)-$(RPM_RELEASE).rpm: $(RPMBUILD_RPMS_x86_64)/lib64rawnet-devel-$(VERSION)-$(RPM_RELEASE).x86_64.rpm
	cp $< $@
