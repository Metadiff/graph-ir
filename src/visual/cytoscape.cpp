//
// Created by alex on 19/10/16.
//

#include "metadiff.h"

namespace md{
    namespace cytoscape{
        std::string polynomial_js(sym::Polynomial polynomial);
        std::string monomial_js(sym::Monomial monomial);

        std::string shape_js(Shape shape){
            return "(" + polynomial_js(shape[0]) + "," + polynomial_js(shape[1]) + "," +
                   polynomial_js(shape[2]) + "," + polynomial_js(shape[3]) + ")";
        }

        std::string clear_full_name(std::string full_name){
            std::string cleaned = full_name;
            auto index = 0;
            while (true) {
                /* Locate the substring to replace. */
                index = cleaned.find(".", index);
                if (index == std::string::npos) break;

                /* Make the replacement. */
                cleaned.replace(index, 1, "_g_");

                /* Advance index forward so the next iteration doesn't pick it up as well. */
                index += 3;
            }
            return cleaned;
        }

        void write_graph(Graph g, std::ostream& s){
            write_header(g->name, s);
            s << "                elements: [\n";
            auto result = write_nodes(g, s);
            write_edges(result.first, s);
            write_groups(result.second, s);
            s << "      ]\n            });\n";
            write_footer(g->name,s);
        };

        std::pair<Edges, GroupMap> write_nodes(Graph g, std::ostream& s){
            Edges edges;
            GroupMap groups;
            s << "// Nodes" << std::endl;
            for(auto i=0; i < g->nodes.size(); ++i) {
                auto node = g->nodes[i];
                auto group = node->group.lock();
                if (groups.find(group->base_full_name()) == groups.end()) {
                    groups.insert({group->base_full_name(), group});
                }
                auto parents = node->op->get_parents();
                auto children = node->children;
                for (auto j = 0; j < parents.size(); ++j) {
                    edges.push_back({parents[j]->id, node->id});
                }
                s << "{data: {" << std::endl
                  << "    id: 'n_" << node->id << "'," << std::endl
                  << "    label: '" << node->op->name << "[" << node->id << "]'," << std::endl
                  << "    parent: 'g_" << clear_full_name(node->group.lock()->base_full_name()) << "'," << std::endl
                  << "    shape: 'ellipse'," << std::endl
                  << "    expanded: 'false'," << std::endl
                  << "    Name: '" << node->name << "'," << std::endl
                  << "    Type: '" << node->node_type << "'," << std::endl
                  << "    Data: '" << node->data_type << "'," << std::endl
                  << "    Shape: '" << shape_js(node->shape) << "'," << std::endl
                  << "    Parents: '[";
                for(auto j=0; j<parents.size(); ++j){
                    if(j == parents.size() - 1){
                        s << parents[j]->id;
                    } else {
                        s << parents[j]->id << ", ";
                    }
                }
                s << "]'," << std::endl
                  << "    Children: '[";
                for(auto j=0; j<children.size(); ++j){
                    if(j == children.size() - 1){
                        s << children[j]->id;
                    } else {
                        s << children[j]->id << ", ";
                    }
                }
                s << "]'," << std::endl
                  << "    group: 'false'" << std::endl
                  << "}}," << std::endl;
            }
            return {edges, groups};
        };

        void write_edges(Edges& edges, std::ostream& s){
            s << "// Edges" << std::endl;
            for(auto i=0; i < edges.size(); ++i){
                s << "{data: {" << std::endl
                  << "   id: 'e_" << edges[i].first << "_" << edges[i].second << "'," << std::endl
                  << "   source: 'n_" << edges[i].first << "'," << std::endl
                  << "   target: 'n_" << edges[i].second << "'" << std::endl
                  << "}}," << std::endl;
            }
        }

        void write_groups(GroupMap& groups, std::ostream& s){
            s << "// Groups" << std::endl;
            for(auto i = groups.begin(); i != groups.end(); ++i) {
                auto g = (*i).second.lock();
                s << "{data: {" << std::endl
                  << "    id: 'g_" << clear_full_name(g->base_full_name()) << "'," << std::endl
                  << "    label: '" << g->name << "'," << std::endl;
                if(not g->is_base()){
                    s << "    parent: 'g_" << clear_full_name(g->parent.lock()->base_full_name()) << "'," << std::endl;
                }
                s << "    shape: 'roundrectangle'," << std::endl
                  << "    group: 'true'" << std::endl
                  << "}}," << std::endl;
            }
        }

        void write_header(std::string name, std::ostream& s){
            s <<    "<!doctype html>\n"
                    "<html>\n"
                    "<head>\n"
                    "    <title>" << name << "</title>\n"
                      "    <!-- jQuery -->\n"
                      "    <script src=\"http://code.jquery.com/jquery-2.0.3.min.js\"></script>\n"
                      "    <!-- Cytoscape -->\n"
                      "    <script src=\"http://cytoscape.github.io/cytoscape.js/api/cytoscape.js-latest/cytoscape.min.js\"></script>\n"
                      "    <!-- Dagre -->\n"
                      "    <script src=\"https://cdn.rawgit.com/cpettitt/dagre/v0.7.4/dist/dagre.min.js\"></script>\n"
                      "    <!-- Dagre extension -->\n"
                      "    <script src=\"https://cdn.rawgit.com/cytoscape/cytoscape.js-dagre/1.3.0/cytoscape-dagre.js\"></script>\n"
                      "    <!-- Cose extension -->\n"
                      "    <script src=\"https://cdn.rawgit.com/cytoscape/cytoscape.js-cose-bilkent/1.3.6/cytoscape-cose-bilkent.js\"></script>\n"
                      "    <!-- Expand extension -->\n"
                      "    <script src=\"cytoscape-expand-collapse.js\"></script>\n"
                      "    <!-- Navigator extension -->\n"
                      "    <link href=\"cytoscape.js-navigator.css\" rel=\"stylesheet\" type=\"text/css\" />\n"
                      "    <script src=\"cytoscape-navigator.js\"></script>\n"
                      "    <!-- Qtip extension -->\n"
                      "    <link rel=\"stylesheet\" type=\"text/css\" href=\"http://cdnjs.cloudflare.com/ajax/libs/qtip2/2.2.0/jquery.qtip.css\">\n"
                      "    <script src=\"http://cdnjs.cloudflare.com/ajax/libs/qtip2/2.2.0/jquery.qtip.js\"></script>\n"
                      "    <script src=\"cytoscape-qtip.js\"></script>\n"
                      "    <style>\n"
                      "        body {\n"
                      "            font-family: helvetica neue, helvetica, liberation sans, arial, sans-serif;\n"
                      "            font-size: 14px;\n"
                      "        }\n"
                      "\n"
                      "        #cy {\n"
                      "            width: 100%;\n"
                      "            height: 90%;\n"
                      "            position: absolute;\n"
                      "            top: 9%;\n"
                      "            bottom: 1%;\n"
                      "            left: 0px;\n"
                      "        }\n"
                      "\n"
                      "        h1 {\n"
                      "            opacity: 0.5;\n"
                      "            font-size: 1em;\n"
                      "            font-weight: bold;\n"
                      "        }\n"
                      "    </style>\n"
                      "\n"
                      "    <script>\n"
                      "        document.addEventListener('DOMContentLoaded', function(){\n"
                      "\n"
                      "            var cy = window.cy = cytoscape({\n"
                      "                container: document.getElementById('cy'),\n"
                      "                style: cytoscape.stylesheet()\n"
                      "                        .selector('node')\n"
                      "                        .css({\n"
                      "                            'shape': 'data(shape)',\n"
                      "                            'content': 'data(label)',\n"
                      "                            'border-width': 2,\n"
                      "                            'border-color': '#000',\n"
                      "                            'text-valign': 'center',\n"
                      "                            'width': 'label',\n"
                      "                            'height': 'label',\n"
                      "                            'padding-left': 6,\n"
                      "                            'padding-right': 6,\n"
                      "                            'padding-top': 5,\n"
                      "                            'padding-bottom': 5\n"
                      "                        })\n"
                      "                        .selector('edge')\n"
                      "                        .css({\n"
                      "                            'target-arrow-shape': 'triangle',\n"
                      "                            'width': 2,\n"
                      "                            'line-color': 'darkorange',\n"
                      "                            'target-arrow-color': '#000',\n"
                      "                            'curve-style': 'bezier'\n"
                      "                        })\n"
                      "                        .selector(':parent')\n"
                      "                        .css({\n"
                      "                            'border-color': '#111',\n"
                      "                            'background-opacity': 0.6,\n"
                      "                            'text-valign': 'top'\n"
                      "                        })\n"
                      "                        .selector('.highlighted')\n"
                      "                        .css({\n"
                      "                            'background-color': '#61bffc',\n"
                      "                            'line-color': '#61bffc',\n"
                      "                            'target-arrow-color': '#61bffc',\n"
                      "                            'transition-property': 'background-color, line-color, target-arrow-color',\n"
                      "                            'transition-duration': '0.5s'\n"
                      "                        }),\n"
                      "                layout: { name: 'dagre' },\n";
        }

        void write_footer(std::string name, std::ostream& s){
            s <<    "            cy.elements().filter('node[group = \"false\"]').qtip({\n"
                    "                content: function(){\n"
                    "                    return \"Name: \" + this.data('Name') + \"<br/>\" +\n"
                    "                            \"Type: \" + this.data('Type') + \"<br/>\" +\n"
                    "                            \"Data: \" + this.data('Data') + \"<br/>\" +\n"
                    "                            \"Shape: \" + this.data('Shape') + \"<br/>\" +\n"
                    "                            \"Parents: \" + this.data('Parents') + \"<br/>\" +\n"
                    "                            \"Children: \" + this.data('Children');\n"
                    "                },\n"
                    "                position: {\n"
                    "                    my: 'top center',\n"
                    "                    at: 'bottom center'\n"
                    "                },\n"
                    "                style: {\n"
                    "                    classes: 'qtip-bootstrap',\n"
                    "                    tip: {\n"
                    "                        width: 16,\n"
                    "                        height: 8\n"
                    "                    }\n"
                    "                }\n"
                    "            });\n"
                    "\n"
                    "            cy.expandCollapse({\n"
                    "                fisheye: true,\n"
                    "                animate: true,\n"
                    "                undoable: false\n"
                    "            });\n"
                    "\n"
                    "            cy.nodes().on(\"afterExpand\", function(event) {\n"
                    "                if(this.data('expanded') == 'false'){\n"
                    "                    this.data('expanded', 'true');\n"
                    "                    node.children().layout(\n"
                    "                            {\n"
                    "                                name: \"breadthfirst\",\n"
                    "                                animate: true,\n"
                    "                                randomize: false,\n"
                    "                                fit: false\n"
                    "                            }\n"
                    "                    );\n"
                    "                }\n"
                    "            });\n"
                    "\n"
                    "            cy.navigator({});\n"
                    "\n"
                    "            document.getElementById(\"collapseRecursively\").addEventListener(\"click\", function () {\n"
                    "                cy.$(\":selected\").collapseRecursively();\n"
                    "            });\n"
                    "\n"
                    "            document.getElementById(\"expandRecursively\").addEventListener(\"click\", function () {\n"
                    "                cy.$(\":selected\").expandRecursively();\n"
                    "            });\n"
                    "\n"
                    "            document.getElementById(\"collapseAll\").addEventListener(\"click\", function () {\n"
                    "                cy.collapseAll();\n"
                    "            });\n"
                    "\n"
                    "            document.getElementById(\"expandAll\").addEventListener(\"click\", function () {\n"
                    "                cy.expandAll();\n"
                    "            });\n"
                    "\n"
                    "            cy.nodes(\":parent\").orphans().collapseRecursively();"
                    "        });\n"
                    "    </script>\n"
                    "</head>\n"
                    "<body >\n"
                    "    <h1>" << name << "</h1>\n"
                      "    <b id=\"collapseAll\" style=\"cursor: pointer;color: darkred\">Collapse all</b> / <b id=\"expandAll\" style=\"cursor: pointer; color: darkslateblue\">Expand all</b> <br/>\n"
                      "    <b id=\"collapseRecursively\" style=\"cursor: pointer; color: darksalmon\">Collapse selected recursively</b> / <b id=\"expandRecursively\" style=\"cursor: pointer; color: darkmagenta\">Expand selected recursively</b>\n"
                      "    <div id=\"cy\"></div>\n"
                      "</body>\n"
                      "</html>\n";
        }

        std::string monomial_js(sym::Monomial monomial){
            if (monomial.powers.size() == 0) {
                return std::to_string(monomial.coefficient);
            }
            std::string result;
            if (monomial.coefficient != 1) {
                if (monomial.coefficient == -1) {
                    result += "-";
                } else {
                    result += std::to_string(monomial.coefficient);
                }
            }
            std::pair <I, std::pair<sym::Polynomial, sym::Polynomial>> floor_var, ceil_var;
            for (auto i = 0; i < monomial.powers.size(); ++i) {
                if ((floor_var = sym::Monomial::registry->get_floor(monomial.powers[i].first)).first != 0) {
                    result += "floor(" + polynomial_js(floor_var.second.first) + " / " +
                              polynomial_js(floor_var.second.second) + ")";
                } else if ((ceil_var = sym::Monomial::registry->get_ceil(monomial.powers[i].first)).first != 0) {
                    result += "ceil(" + polynomial_js(ceil_var.second.first) + " / " +
                              polynomial_js(ceil_var.second.second) + ")";
                } else {
                    result += ('a' + monomial.powers[i].first);
                    auto n = monomial.powers[i].second;
                    if(n != 1) {
                        result += "' + '" + std::to_string(n) + "'.sup() + '";
                    }
                }
            }
            return result;
        }

        std::string polynomial_js(sym::Polynomial polynomial){
            if (polynomial.monomials.size() == 0) {
                return "0";
            }
            std::string result = monomial_js(polynomial.monomials[0]);
            for (auto i = 1; i < polynomial.monomials.size(); ++i) {
                if (polynomial.monomials[i].coefficient > 0) {
                    result += "+" + monomial_js(polynomial.monomials[i]);
                } else {
                    result += monomial_js(polynomial.monomials[i]);
                }
            }
            return result;
        }
    }
}