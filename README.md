Adaptive Sequence Memorizer(ASM)
===========================

Storing and recalling sequences are necessary for making predictions, recognizing time-based patterns and generating behaviour. Jeff Hawkins et.al. in [1] has proposed that these tasks are major functions of the neocortex, the ability to store and recall time-based sequences is probably a key attribute of many, if not all, cortical areas.

On 2011, Hawkins et. al. has proposed a new algorithm called Hierarchical Temporal Memory including Cortical Learning Algorithm (HTM CLA) [2]. there are many open and closed source implmentation of this algorithm including [NuPIC](https://github.com/numenta/nupic), [OpenHTM](https://sourceforge.net/projects/openhtm/), etc.

HTM CLA has two main features the Spatial Pooler and the Temporal Pooler where in Spatial Pooler input patterns will be mapped to Invariant Sparse Representations and in Temporal Pooler time-basnted patterns will be extracted and memorized. As HTM CLA is based on randomness and sparsity it can not memorize and retrieve exact patterns which is essential in some problems. In other words HTM CLA memorizes patterns and their sequences in a loozy approach and there are no way to convert this to a looseless approach.

**Adaptive Sequence memorizer** is an special implementation of HTM CLA where just the Temporal Pooler is implemented and can be used to memorize and retrieve sequetial patterns each one marked with an ID. Based on CLA it can memorize infinite sequences in an small amount of memory and retrieve them from any desired point of the sequence. This is something like [Auto-Associative Memories](http://en.wikipedia.org/wiki/Autoassociative_memory) but with better algorithm complexity and memory usage.



## Build and test ASM:

Dependencies:
 * GCC (+4.8)
 * Make or any IDE supported by CMake (Visual Studio, Eclipse, XCode, KDevelop, etc)


#### Configure and generate build files:

    mkdir -p $ASM/build
    cd $ASM/build
    cmake ..

#### Build:

    cd $ASM/build
    make

#### Run the tests:

    cd $ASM/build/test
    ./test

###References
[1]: Hawkins, J., George, D., & Niemasik, J. (2009). *Sequence memory for prediction, inference and behaviour.* Philosophical Transactions of the Royal Society B: Biological Sciences, 364(1521), 1203-1209.

[2]: [HTM Cortical Learning Algorithm](http://numenta.org/resources/HTM_CorticalLearningAlgorithms.pdf)
