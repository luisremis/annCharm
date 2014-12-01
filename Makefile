CHARMC = /home/acun2/charm/net-linux-x86_64/bin/charmc $(OPTS)
#CHARMC = /Users/luisremis/charm-6.6.0/bin/charmc $(OPTS)
TESTOPTS =  +p4 ++local

OBJS = neuralNetwork.o

#inputNeurons = 2
#nMiddleLayers = 1
#middleLayersNeurons = 4
#outputNeurons = 2
#neuronsPerChare = 1
#inputVecFile=data/train_imgs_small.mat
#inputLabelFile=data/train_labels_small.mat
#testVecFile=data/train_imgs_small.mat
#testLabelFile=data/train_labels_small.mat

inputNeurons = 900
nMiddleLayers = 1
middleLayersNeurons = 90
outputNeurons = 10
neuronsPerChare = 10
inputVecFile=data/train_imgs_0_1_900dimMixed20.mat
inputLabelFile=data/train_labels_0_1_900dimMixed20.mat
testVecFile=data/test_imgs_0_1_900dimMixed.mat
testLabelFile=data/train_labels_0_1_900dimMixed.mat

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

testfile: clean
	make all
	./charmrun neuralNetwork $(inputNeurons) $(nMiddleLayers) \
	$(middleLayersNeurons) $(outputNeurons) $(neuronsPerChare) \
	$(inputVecFile) $(inputLabelFile) \
	$(testVecFile) $(testLabelFile) $(TESTOPTS)
