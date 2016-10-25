# Graph Intermediate Representation 

# A guide to operators

The operators are split into hierarchy starting from one abstract class.
Additionally operators can have several of predefined traits. The traits
are implemented via virtual inheritance and define some specific behaviour of the operator.
Below are listed all of the traits. 

## AbstractOperator 

### Virtual methods:

* *is_input_dependent* - returns the true if any of the arguments is input dependent, otherwise false
* *is_differentiable* - returns the true if any of the parents is differentiable, otherwise false
* *get_data_type* - returns the **dataType** based on the parents.
* *get_arguments* - returns the all nodes which affect the output of the operator in a non-differentiable manner.
* *equals* - returns true only if the operator is symbolically equal to each other (Not implemented yet)

### Pure virtual methods:

* *copy_to* - copies the operator to a new graph, given the corresponding ancestors
* *get_shape* - returns the **Shape** of the output of the operator.
* *get_parents* - returns the all nodes which affect the output of the operator in a differentiable manner.
* *get_parent_grad* - returns the gradient of the parent, given the gradient of the output.

### Implemented methods:

* *get_ancestors* - returns the union of parents and arguments
* *send_grad_message* - adds the gradient from this node to the existing one for the parent or creates a new one
* *generate_gradients*  - given the gradient of the output generates all gradients of the parents
* *get_grad_level* - returns the maximal from the ancestors

## Operator traits

Operator traits are represented as Pure Virtual subclasses of **AbstractOperator**. 
The are non-disjoint traits which different operators can have and define their some of their behaviour.
They are combined together for each individual operators.

### NonDifferentiableOperator
An operator which is always non differentiable.

Overloaded methods:

* *is_differentiable* - returns false


### OrphanOperator 
An operator who has no parents.

Overloaded methods:

* *get_parents* - returns an empty list
* *get_parent_grad* - throws an error


### InputOperator [OrphanOperator]
An operator which is an input to the graph.

Overloaded methods:
* *is_input_dependent* - returns true
* *is_differentiable* - returns true


### ConstantOperator [OrphanOperator, NonDifferentiableOperator]
An operator which is a constant expression. 
Takes a **dataType** in the constructor.

Overloaded methods:

* *get_data_type* - returns the **dataType** provided


### LogicalOperator [NonDifferentiableOperator]
A logical expression operator

Overloaded methods:

* *get_data_type* - returns the **b8**
* *get_parent_grad* - throws an error


### IntegerOperator
An operator which always returns integer values

Overloaded methods:

* *get_dada_type* - returns an integer 


### FloatOperator
An operator which always returns float values

Overloaded methods:

* *get_dada_type* - returns **max_float**


### ElementwiseOperator
An operator which is applied elementwise to its inputs

Overloaded methods:

* *get_shape* - returns the first parent's shape


### UnaryOperator
An operator which has a single parent.
Takes the parent in the constructor. 

Overloaded methods:

* *get_parents* - returns the parent provided.


### BinaryOperator
An operator which has exactly two parents.
Takes the parents in the constructor. 

Overloaded methods:

* *get_parents* - returns the two parents.


### AssociativeOperator
An operator which can have two or more parents.
Takes the parents in the constructor, should be at least two.

Overloaded methods:

* *get_parents* - returns the parents.


### MorphOperator [UnaryOperator]
An unary operator which do not change the **dataType** of the parent.

Overloaded methods:

* *get_data_type* - returns the **dataType** of the parent.


### ReductionOperator [UnaryOperator]
A reduction operation along some axes. Takes the axes in the constructor.

Overloaded methods:

* *get_shape* - returns the **Shape** based on the shape of the parent and the axes provided.






  
