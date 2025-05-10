#include "Langauge.h"

Map_variables map_variables_init() {
    return (Map_variables) {
        .hash_variable_tuples = array_init(Array_type_tuple__hash_variable),
    };
}

void map_variables_delete(Map_variables* map) {
    for (int i=0; i<map->hash_variable_tuples.length; ++i) {
        Tuple__hash_variables* tuple = 
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr);
        
            for (int j=0; j<tuple->variables.length; ++j) {
                Variable* var = 
                    ((Variable*) tuple->variables.arr);

                string_delete(&var->name);
            }
        array_delete(&tuple->variables);
    }

    array_delete(&map->hash_variable_tuples);
}


int map_variables_hash(String name) {
    return name.length;
}

void map_variables_add(Map_variables* map, String name, Evaluation value) {
    // Getting hash
    int hash = map_variables_hash(name);
    printf("Hash: %d \n", hash);

    // Searching for the index of hash value
    int tuple_idx_for_hash = -1;
    for (int i=0; i<map->hash_variable_tuples.length; ++i) {
        Tuple__hash_variables* tuple_arr = 
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr);
        if (tuple_arr[i].hash == hash) {
            tuple_idx_for_hash = i;
            break;
        }
    }
    printf("tuple_idx_for_hash: %d \n", tuple_idx_for_hash);


    // Not found
    if (tuple_idx_for_hash == -1) {
        Tuple__hash_variables new_tuple = (Tuple__hash_variables) {
            .hash      = hash,
            .variables = array_init(Array_type_variable),
        };
        Variable new_var = (Variable) { .name = name, .value = value };
        array_add(&new_tuple.variables, (void*) &new_var, Array_type_variable); 
        
        array_add(&map->hash_variable_tuples, (void*) &new_tuple, Array_type_tuple__hash_variable); 
    }
    else { // Found
        Tuple__hash_variables tuple = ((Tuple__hash_variables*) map->hash_variable_tuples.arr)[tuple_idx_for_hash];
        Variable new_var            = (Variable) { .name = name, .value = value };
        
        array_add(&tuple.variables, (void*) &new_var, Array_type_variable); 

        // TODO: make it not copy like that all the time
        ((Tuple__hash_variables*) map->hash_variable_tuples.arr)[tuple_idx_for_hash] = tuple;
    }

    printf("Current map varaibles: \n");
    for (int i=0; i<map->hash_variable_tuples.length; ++i) {
        Tuple__hash_variables tuple =
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr)[i];

        for (int j=0; j<tuple.variables.length; ++j) {
            Variable var = ((Variable*)tuple.variables.arr)[j];
            printf("\tVariable: ");
            string_print(&var.name);
            printf("\n");
        }
    }

}

Variable map_variables_get(Map_variables* map, String name) {
    int hash = map_variables_hash(name);

    int tuples_len = map->hash_variable_tuples.length;
    Tuple__hash_variables* tuples = 
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr);
    
    for (int i=0; i<tuples_len; ++i) {
        if (tuples[i].hash == hash) {
            int variables_len   = tuples[i].variables.length; 
            Variable* variables = 
                ((Variable*) tuples[i].variables.arr);
            
            for (int j=0; j<variables_len; ++j) {
                if (variables[j].name.str == name.str) {
                    return variables[j];
                }
            }
            
        } 
    }

    printf("Was trying to acces a value for an unexsistant variable. \n");
    printf("Identifier: %s. \n", name.str);
    exit(1);
}


















