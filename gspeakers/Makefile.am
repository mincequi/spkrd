SUBDIRS = src
install-data-local:	
	@$(NORMAL_INSTALL)
	if test -d xml; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/xml; \
	  for xml in xml/*; do \
	    if test -f $$xml; then \
	      $(INSTALL_DATA) $$xml $(DESTDIR)$(pkgdatadir)/xml; \
	    fi \
	  done \
	fi 
	if test -d xpm; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/xpm; \
	  for xpm in xpm/*; do \
	    if test -f $$xpm; then \
	      $(INSTALL_DATA) $$xpm $(DESTDIR)$(pkgdatadir)/xpm; \
	    fi \
	  done \
	fi 

