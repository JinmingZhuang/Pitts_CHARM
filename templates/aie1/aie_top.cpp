{% for layer in L_list -%}
#include "layer{{layer}}/aie_top_L{{layer}}.h"
{% endfor -%}

using namespace adf;

{% for layer in L_list -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
{% set A_BRO = HW_Conf[layer][3] -%}
{% for i in range(A) -%}
{% for j in range(C//A_BRO) -%}
{% for k in range(B) -%}
{% set row = k+j*B+i*(C//A_BRO)*B -%}
PLIO* LHS_in{{row}}_L{{layer}} = new PLIO("LHS_in{{row}}_L{{layer}}", adf::plio_{{port_width}}_bits, "data/input0.txt",{{freq}});
{% endfor -%}
{% endfor -%}
{% endfor %}
{% endfor -%}

{% for layer in L_list -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
{% set C_BRO = HW_Conf[layer][4] -%}
{% for j in range(C) -%}
{% for i in range(A//C_BRO) -%}
{% for k in range(B) -%}
{% set col = k+i*B+j*(A//C_BRO)*B -%}
PLIO* RHS_in{{col}}_L{{layer}} = new PLIO("RHS_in{{col}}_L{{layer}}", adf::plio_{{port_width}}_bits, "data/input1.txt",{{freq}});
{% endfor -%}
{% endfor -%}
{% endfor -%}
{% endfor -%}


{% for layer in L_list -%}
{% set port_pre = Port_Conf_Pre[layer][2] -%}
{% set A = HW_Conf[layer][0] -%}
{% set C = HW_Conf[layer][2] -%}
{% for i in range(A) -%}
{% for j in range(C) -%}
{% set out = j+i*C -%}
PLIO* out{{j+i*C}}_L{{layer}} = new PLIO("out{{j+i*C}}_L{{layer}}", adf::plio_{{port_width}}_bits, "data/output{{out + port_pre}}.txt",{{freq}});
{% endfor -%}
{% endfor %}
{% endfor -%}

simulation::platform<{{port_total[0]+port_total[1]}}, {{port_total[2]}}> platform(
{% for layer in L_list -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
{% set A_BRO = HW_Conf[layer][3] -%}
{% set C_BRO = HW_Conf[layer][4] -%}
{% for i in range(A*(C//A_BRO)*B) -%} 
LHS_in{{i}}_L{{layer}},
{% endfor-%}
{% endfor -%}

{% for layer in L_list -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
{% set A_BRO = HW_Conf[layer][3] -%}
{% set C_BRO = HW_Conf[layer][4] -%}
{% for i in range(C*(A//C_BRO)*B) -%} 
RHS_in{{i}}_L{{layer}}, 
{% endfor -%}
{% endfor -%}

{% for layer in L_list -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
{% set A_BRO = HW_Conf[layer][3] -%}
{% set C_BRO = HW_Conf[layer][4] -%}
{% for i in range(A*C) -%} 
{% if layer==L_list[-1] and i==(A*C-1) -%}
out{{i}}_L{{layer}} 
{% else -%}
out{{i}}_L{{layer}},
{% endif-%}
{% endfor-%}
{% endfor-%}
);


{% for layer in L_list -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
mm_x{{A}}_x{{B}}_x{{C}}_graph{{layer}}  mm_graph{{layer}};
{% endfor %}

{% for layer in L_list -%}
{% set port_pre = Port_Conf_Pre[layer][0] -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
{% set A_BRO = HW_Conf[layer][3] -%}
{% for i in range(A) -%}
{% for j in range(C//A_BRO) -%}
{% for k in range(B) -%}
{% set row = k+j*B+i*(C//A_BRO)*B -%}
connect<> net_lhs_in{{row}}_L{{layer}} (platform.src[{{row + port_pre}}], mm_graph{{layer}}.in_lhs[{{j+i*(C//A_BRO)}}][{{k}}]);
{% endfor -%}
{% endfor -%}
{% endfor %}
{% endfor -%}

{% for layer in L_list -%}
{% set port_pre = Port_Conf_Pre[layer][1] -%}
{% set A = HW_Conf[layer][0] -%}
{% set B = HW_Conf[layer][1] -%}
{% set C = HW_Conf[layer][2] -%}
{% set C_BRO = HW_Conf[layer][4] -%}
{% for j in range(C) -%}
{% for i in range(A//C_BRO) -%}
{% for k in range(B) -%}
{% set col = k+i*B+j*(A//C_BRO)*B -%}
connect<> net_rhs_in{{col}}_L{{layer}} (platform.src[{{col + port_pre + port_total[0]}}], mm_graph{{layer}}.in_rhs[{{i+j*(A//C_BRO)}}][{{k}}]);
{% endfor -%}
{% endfor -%}
{% endfor -%}
{% endfor -%}

{% for layer in L_list -%}
{% set port_pre = Port_Conf_Pre[layer][2] -%}
{% set A = HW_Conf[layer][0] -%}
{% set C = HW_Conf[layer][2] -%}
{% for i in range(A) -%}
{% for j in range(C) -%}
{% set out = j+i*C -%}
connect<> net_out{{out}}_L{{layer}} (mm_graph{{layer}}.out[{{out}}], platform.sink[{{out + port_pre}}]);
{% endfor -%}
{% endfor -%}
{% endfor -%}

#ifdef __AIESIM__
int main(int argc, char** argv) {
    {% for layer in L_list -%}
    mm_graph{{layer}}.init();
    {% endfor -%}
    
    {% for layer in L_list -%}
    mm_graph{{layer}}.run(4);
    {% endfor -%}

    {% for layer in L_list -%}
    mm_graph{{layer}}.end();
    {% endfor -%}

    
    return 0;
}
#endif