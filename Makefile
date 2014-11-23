#CHARMC = /home/acun2/charm/net-linux-x86_64/bin/charmc $(OPTS)
CHARMC = /Users/luisremis/charm-6.6.0/bin/charmc $(OPTS)
TESTOPTS =  +p4 ++local

OBJS = neuralNetwork.o

inputNeurons = 4
nMiddleLayers = 1
middleLayersNeurons = 6
outputNeurons = 4
neuronsPerChare = 2
#for now, nueronsPerChar must be multiple of nuerons in each layer.

all: neuralNetwork

neuralNetwork: $(OBJS)
	$(CHARMC) -O3 -language charm++ -o neuralNetwork $(OBJS)

neuralNetwork.decl.h: neuralNetwork.ci
	$(CHARMC)  neuralNetwork.ci

clean:
	rm -f *.decl.h *.def.h conv-host *.o neuralNetwork charmrun

cleanp:
	rm -f *.sts *.gz *.projrc *.topo *.out

neuralNetwork.o: neuralNetwork.C neuralNetwork.decl.h
	$(CHARMC) -c neuralNetwork.C

test: all
	./charmrun neuralNetwork $(inputNeurons) $(nMiddleLayers) $(middleLayersNeurons) $(outputNeurons) $(neuronsPerChare) $(TESTOPTS)