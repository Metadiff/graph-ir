# Graph Intermediate Representation 

# A guide to the repository and the project

# A guide to the operators

The operators are split into hierarchy starting from one abstract class.
Below I will list all abstract operators and which virtual (abstract) 
functions they override and how as to make it easy for anyone to understand
each individual operators.


**AbstractOperator** 
This is the top level class.

Virtual methods:

* *get_node_type* - returns the **nodeType** based on the parents and arguments
* *get_data_type* - returns the **dataType** based on the parents
* *get_arguments* - returns the all nodes which affect the output of the operator in a non-differentiable manner.
* *equals* - returns true only if the operator is symbolically equal to each other (Not implemented yet)

Pure virtual methods:

* *copy_to* - copies the operator to a new graph, given the corresponding ancestors
* *get_shape* - returns the **Shape** of the output of the operator.
* *get_parents* - returns the all nodes which affect the output of the operator in a differentiable manner.
* *get_parent_grad* - returns the gradient of the parent, given the gradient of the output.

Implemented methods:

* *get_ancestors* - returns the union of parents and arguments
* *send_grad_message* - adds the gradient from this node to the existing one for the parent
* *generate_gradients*  - given the gradient of the output generates all gradients of the parents
* *get_grad_level* - returns the maximal from the parents


**OrphanOperator [AbstractOperator]**
This is an abstract operator encapsulating all operators which don't have any parents.
It takes a **dataType** as an argument. 

Overloaded methods:

* *get_data_type* - returns the **dataType** provided as argument
* *get_parents* - returns an empty list
* *get_parent_grad* - throws an error

*Examples:* Input and Constant operators.


**InputOperator [OrphanOperator]**
This is an abstract operator encapsulating all operators which describe an input. 

Overloaded methods:
* *get_node_type* - returns **INPUT** 

*Examples:* Inputs, Shared variables.

**ConstantOperator [OrphanOperator]**
This is an abstract operator encapsulating all operators which describe a constant expression.
Note that this also includes constant expressions dependable on inputs.

Overloaded methods:

* *get_node_type* - returns **CONSTANT** or **INPUT_DERIVED_NON_DIFF** 

*Examples:* Eye, Range, Constant values.

**LogicalOperator [AbstractOperator]**
This is an abstract operator encapsulating all logical operators.

Overloaded methods:

* *get_node_type* - returns the **CONSTANT_DERIVED** or **INPUT_DERIVED_NON_DIFF** 
* *get_data_type* - returns the **b8**
* *get_parent_grad* - throws an error

*Examples:* LogicalUnary, LogicalBinary

**LogicalUnary [LogicalOperator]**
This is an abstract operator encapsulating all unary logical operators.

Overloaded methods:

* *get_shape* - returns the shape of the parent
* *get_parents* - returns the parent

*Examples:* LogicalNot, IsNaN, IsInf


**NaryOperator [AbstractOperator]**
This is an abstract operator encapsulating all operators which describe an
elementary operation which can be applied to more than two nodes. Takes
 all parents and the resulting shape as an argument.

Overloaded methods:
* *get_node_type* - returns according to the parents' types
* *get_data_type* - returns according to the parents' data types
* *get_shape* - returns the shape provided
* *get_parents* - returns the parents provided
* *get_arguments* - returns an empty list


**BinaryOperator [AbstractOperator]**
This is an abstract operator encapsulating all operators which take exactly
two nodes as inputs. Takes the two parents and the resulting shape as inputs.

Overloaded methods:
* *get_node_type* - returns according to the parents' types
* *get_shape* - returns the shape provided
* *get_parents* - returns the parents provided
* *get_arguments* - returns an empty list






  
