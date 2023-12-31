#include "glut_ply.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include <fstream>

Model_PLY::Model_PLY() {
    centro = vec3(0.0);
    escala = 1;
}

int Model_PLY::Load(char *filename) {
    centro = vec3(0);
    char *pch = strstr(filename, ".ply");
    int cantVertices, cantIndices;
    float val[6];
    int ind[3];
    if (pch != NULL) {
        positions.clear();
        normals.clear();
        indices.clear();
        FILE *file = fopen(filename, "r");
        if (file) {
            int i = 0;
            int normal_index = 0;
            char buffer[1000];
            fgets(buffer, 1000, file);            // ply
            // READ HEADER
            // Find number of vertexes
            while (strncmp("element vertex", buffer, strlen("element vertex")) != 0) {
                fgets(buffer, 300, file);            // format
            }
            strcpy(buffer, buffer + strlen("element vertex"));
            sscanf(buffer, "%i", &cantVertices);
            // Find number of faces
            fseek(file, 0, SEEK_SET);
            while (strncmp("element face", buffer, strlen("element face")) != 0) {
                fgets(buffer, 300, file);            // format
            }
            strcpy(buffer, buffer + strlen("element face"));
            sscanf(buffer, "%i", &cantIndices);

            // go to end_header
            while (strncmp("end_header", buffer, strlen("end_header")) != 0) {
                fgets(buffer, 300, file);            // format
            }

            // read vertices
            i = 0;
            int iterator;
            float min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN, max_z = FLT_MIN;
            float total_x = 0, total_y = 0, total_z = 0, distance, max_d = 0;
            std::vector<vec3> vertices;
            for (iterator = 0; iterator < cantVertices; iterator++) {
                fgets(buffer, 300, file);
                sscanf(buffer, "%f %f %f %f %f %f",
                       &val[i], &val[i + 1], &val[i + 2],
                       &val[i + 3], &val[i + 4], &val[i + 5]);
                vertices.emplace_back(vec3(val[i], val[i + 1], val[i + 2]));
                total_x += val[i];
                total_y += val[i + 1];
                total_z += val[i + 2];
                positions.emplace_back(vec3(val[i], val[i + 1], val[i + 2]));
                normals.emplace_back(vec3(val[i + 3], val[i + 4], val[i + 5]));
            }
            //vec3 nuevo_centro((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + min_z) / 2);
            float cx = total_x / cantVertices;
            float cy = total_y / cantVertices;
            float cz = total_z / cantVertices;
            vec3 nuevo_centro = vec3(cx, cy, cz);
            this->bs.centro = nuevo_centro;
            this->centro = nuevo_centro;
            for (iterator = 0; iterator < cantVertices; iterator++) {
                distance = sqrt(pow(vertices[iterator].x - cx, 2) + pow(vertices[iterator].y - cy, 2) +
                                pow(vertices[iterator].z - cz, 2));
                if(distance > max_d) {
                    max_d = distance;
                }
            }
            this->bs.radio = max_d;
            //this->bs.radio = std::max(std::max((max_x - min_x) / 2, (max_y - min_y) / 2), (max_z - min_z) / 2);
            // read faces
            i = 0;
            for (iterator = 0; iterator < cantIndices; iterator++) {
                fgets(buffer, 300, file);
                if (buffer[0] == '3') {
                    buffer[0] = ' ';
                    sscanf(buffer, "%i %i %i", &ind[i], &ind[i + 1], &ind[i + 2]);
                    indices.emplace_back(ind[i]);
                    indices.emplace_back(ind[i + 1]);
                    indices.emplace_back(ind[i + 2]);
                    //std::cout << Indices[i] << "\t" << Indices[i+1] << "\t" << Indices[i+2] << std::endl;
                }
            }
            indices_size = indices.size();
            fclose(file);
            printf("Finish!!\n");
        } else {
            printf("File can't be opened\n");
        }
    } else {
        printf("File does not have a .PLY extension. ");
    }
    return 0;
}

void Model_PLY::imprimir() {
    cout << "\nVertices:\n";
    for (int i = 0; i < positions.size(); i++) {
        cout << positions[i].x << " " << positions[i].y << " " << positions[i].z << " ";
        cout << normals[i].x << " " << normals[i].y << " " << normals[i].z << endl;
    }
    cout << "\nIndices: \n";
    for (int i = 0; i < indices.size(); i += 3) {
        cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << endl;
    }
}

int Model_PLY::enviar_GPU() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbos[3];
    glGenBuffers(3, vbos);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return vao;
}

void Model_PLY::display(Shader &sh) {
    model = mat4(1.0);
    //this->bs.radio *= escala;
    model = translate(model, centro + vec3(traslacion));
    model = rotate(model, glm::radians(rotx), vec3(1, 0, 0));
    model = rotate(model, glm::radians(roty), vec3(0, 1, 0));
    model = rotate(model, glm::radians(rotz), vec3(0, 0, 1));
    model = scale(model, vec3(escala));
    //this->bs.centro += vec3(traslacion);
    //std::cout<<centro.x + traslacion<<" "<<centro.y + traslacion<<" "<<centro.z + traslacion<< endl;
    //std::cout<<bs.centro.x + bs.traslacion<<" "<<bs.centro.y + bs.traslacion<<" "<<bs.centro.z + bs.traslacion<< endl;


    sh.setMat4("model", model);
    if (true) {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
