# Graph Intermediate Representation 

The Graph Intermediate Representation **(GIR)** is an abstract 
representation of mathematical computations.

The main motivation comes from the **LLVM** project which revolutionized 
compiler engineering by providing a unified intermediate representation 
(**LLVM-IR**). This allowed the rapid development of optimizations on 
this intermediary representation independent of the front and back end. 
With increasing rise in AI and Machine Learning there have been many tools,
such as Caffe, Theano, Tensorflow, Torch7, CNTK, MXNet and many others, whose main 
goals is to optimize the computation of programs which involve only numerical
calculations on large multi-dimensional arrays. In addition to this they 
provide automatic differentiation capabilities in order to simplify and 
speed up prototype development and research. Unfortunately, each of these 
projects uses their own internal representation for the computation undertaken,
and the maintainers of each package reinvent the wheel by both building 
a similar data structure as well as independently developing different forms 
of optimizations. It is the authors belief that this slows down the development of
the field and also does not help the community to jointly benefit from 
all of the work done in the area.
 
The main goals of the project are:
 
1. Provide a unified and general Graph Intermediate Representation, which to 
capture all possible mathematical computations.
 
2. Provide automatic meta-data inference for all operations, which to allow
the correct choice of down-stream implementations. The meta-data covers
things like: shape, data type, symmetry for matrices, positive definite 
qualifiers for matrices, specially structured matrix variants like 
diagonal, tri-diagonal and etc...
 
3. Provide automatic differentiation as a graph operation for both 
forward and backward mode.
 
4. Provide bindings to different languages in order to allow developers of
 Machine Learning and computational tools in general to easily leverage 
 the **GIR**.


This in terms will faciliate the development of unified optimization procedures
on the **GIR** which to benefit all software packages using it. These will include
optimizations for both single device and distributed computation.
 

## Development of the project

The project is still in its very infancy, however a large part of the project
has already been implemented. The main Graph and Operator representation 
together with the meta-data has been defined and implemented. Forward and 
backward automatic differentiation has been already implemented for most 
of the operators. The main missing bit at this stage is a simple native 
backend which to faciliate unit testing on very small problems. 


### List of operators that have not yet been implemented

1. *MultiOutputOperator* - this will be an abstract operator encapsulating 
all operators which have more than one output.

2. *MultiIndexOperator* - currently each operator can have a single output.
For this the *MultiOutputOperator* will have a single output, but it will 
be indexed by *MultiIndexOperator* and retrieve all of the relevant outputs 
in order. 

3. *SubRoutineOperator* - the subroutine will allow complex computation
to be hidden from the overall graph and to be optimized on its own. 
This will provide a form of encapsulation when the user needs it. 


1. *Convolution* 

2. *MaxPooling*

3. *Indexing*

4. *GradientIndexing*


### The Native Backend

Unit testing and verification of the **GIR** is crucial for the project. 
In order to faciliate this without developing a significantly sophisticated
general backend, a simple native mock C++ backend is needed. This most likely 
will be a highly inefficient and slow for large tasks, however it will 
be sufficient for verification of automatic differentiation rules. 


### Contribution

If anyone is interested in contributing to the project please contact the 
authors at ...


