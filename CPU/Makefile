# Changed this Makefile to generate dependencies automatically
INCLUDEFLAGS = -I. -I../spim

%.o:%.c
	$(CC) -g $(OBJS) $(LDFLAGS) -o $@

%.d:%.c
	@set -e; rm -f $@; $(CC) -MM $< $(INCLUDEFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(OBJS:.o=.d)
