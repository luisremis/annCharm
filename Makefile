CHARMC = /home/acun2/charm/net-linux-x86_64/bin/charmc $(OPTS)
#CHARMC = /Users/luisremis/charm-6.6.0/bin/charmc $(OPTS)
TESTOPTS =  +p8 ++local

OBJS = neuralNetwork.o

inputNeurons = 900
nMiddleLayers = 1
middleLayersNeurons = 100
outputNeurons = 10
neuronsPerChare = 10
inputVecFile=data/train_imgs_0_1_900dimMixed20.mat
inputLabelFile=data/train_labels_0_1_900dimMixed20.mat
testVecFile=data/test_imgs_all_900dim.mat
testLabelFile=data/train_labels_0_1_900dimMixed.mat

all: neuralNetwork

neuralNetwork: $(OBJS)
	$(CHARMC) -O3 -language charm++ \
	-o neuralNetwork $(OBJS)

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

testfile: clean
	make all
	date
	./charmrun neuralNetwork $(inputNeurons) $(nMiddleLayers) \
	$(middleLayersNeurons) $(outputNeurons) $(neuronsPerChare) \
	$(inputVecFile) $(inputLabelFile) \
	$(testVecFile) $(testLabelFile) $(TESTOPTS)
	date
