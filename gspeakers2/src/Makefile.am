bin_PROGRAMS = gspeakers

#List of source files needed to build the executable:
gspeakers_SOURCES = main.cc common.h common.cc gspeakersobject.h gspeakersobject.cc \
part.h part.cc net.h net.cc crossover.h crossover.cc crossoverlist.h \
crossoverlist.cc speaker.h speaker.cc speakerlist.h speakerlist.cc box.h \
box.cc boxlist.h boxlist.cc speakereditor.h speakereditor.cc \
cellitemcrossover.h cellitemcrossover.cc crossovertreeview.h \
crossovertreeview.cc crossoverhistory.h crossoverhistory.cc \
boxeditor.h boxeditor.cc \
boxhistory.h boxhistory.cc gspeakerscolor.h gspeakerscolor.cc \
plothistory.h plothistory.cc gspeakersplot.h gspeakersplot.cc \
gspeakersboxplot.h gspeakersboxplot.cc settings.h settings.cc \
mainwindow.h mainwindow.cc crossoverwizard.h crossoverwizard.cc \
filterlinkframe.h filterlinkframe.cc gspeakersfilterplot.h \
gspeakersfilterplot.cc settingsdialog.h settingsdialog.cc \
totalfilterplot.h totalfilterplot.cc tabwidget.h tabwidget.cc \
freqrespeditor.h freqrespeditor.cc driverfreqrespplot.h driverfreqrespplot.cc

CXXFLAGS = @CXXFLAGS@ $(DEPS_CFLAGS) -Wall -O0
LDADD = $(DEPS_LIBS)