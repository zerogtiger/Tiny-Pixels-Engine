CC				:= g++
TARGET		:= "ImageProcessing"
BUILDDIR	:= build
BUILDLIBDIR	:= build/lib
SRCDIR		:= src
LIBDIR		:= src/lib
CFLAGS		:= -std=c++17 -g
SRCEXT		:= cpp
SOURCES 	:= $(wildcard $(SRCDIR)/*.$(SRCEXT))
LIBSRCS 	:= $(wildcard $(LIBDIR)/*.$(SRCEXT))
OBJECTS		:= $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.o)) $(patsubst $(LIBDIR)/%, $(BUILDLIBDIR)/%, $(LIBSRCS:.$(SRCEXT)=.o))

$(BUILDLIBDIR)/%.o: $(LIBDIR)/%.$(SRCEXT)
	@printf "\e[33m\e[1mBuilding...\e[0m\n";
	@mkdir -p $(BUILDLIBDIR)
	@echo "  $(notdir $@) from $(notdir $<)"
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@printf "\e[33m\e[1mBuilding...\e[0m\n";
	@mkdir -p $(BUILDDIR)
	@echo "  $(notdir $@) from $(notdir $<)"
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	@printf "\e[35m\e[1mLinking...\e[0m\n";
	@echo "  $(notdir $(OBJECTS))"
	$(CC) $(CFLAGS) -o $@ $^


PHONY: clean
clean:
	@printf "\e[31m\e[1mCleaning...\e[0m\n"
	@echo "  /$(BUILDDIR)"
	@echo "  /$(TARGET)"
	@$(RM) -r $(BUILDDIR) $(OBJECTS)
	@$(RM) "./$(TARGET)"

PHONY: r
r:
	@printf "\e[33m\e[1mRunning $(TARGET)\e[0m\n"
	@./$(TARGET)

PHONY: run
run:
	@echo $(OBJECTS)
	@mkdir -p $(BUILDLIBDIR)
	@for source in $(basename $(notdir $(LIBSRCS))); do\
		printf "\e[33m\e[1mBuilding Library...\e[0m\n";\
		echo "  $$source.o from $$source.$(SRCEXT)";\
		$(CC) $(CFLAGS) -c -o $(BUILDLIBDIR)/$$source.o $(LIBDIR)/$$source.$(SRCEXT);\
	done
	@for source in $(basename $(notdir $(SOURCES))); do\
		printf "\e[33m\e[1mBuilding...\e[0m\n";\
		echo "  $$source.o from $$source.$(SRCEXT)";\
		$(CC) $(CFLAGS) -c -o $(BUILDDIR)/$$source.o $(SRCDIR)/$$source.$(SRCEXT);\
	done
	@printf "\e[95m\e[1mLinking...\e[0m\n";
	@echo "  $(notdir $(OBJECTS))"
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@printf "\e[33m\e[1mRunning $(TARGET)\e[0m\n"
	@ ./$(TARGET)

