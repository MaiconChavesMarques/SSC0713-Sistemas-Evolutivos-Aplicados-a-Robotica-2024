#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <utility>

// Número de janelas
const int numWindows = 4;

// Estrutura para armazenar os dados de cada janela
struct WindowData {
    std::vector<std::pair<float, float>> redLinePoints;   // Pontos da linha vermelha
    std::vector<std::pair<float, float>> blueLinePoints;  // Pontos da linha azul
    float lastRedPointX = 0.0f;  // Última posição X da linha vermelha
    float lastBluePointX = 0.0f; // Última posição X da linha azul
};

// Lista que armazena os dados de todas as janelas
std::vector<WindowData> allWindowsData(numWindows);

// Configurações gerais
float pointMoveSpeed = 10.0f;        // Velocidade do deslocamento (em pixels)
int windowWidth = 400;              // Largura da janela
int windowHeight = 300;             // Altura da janela
float screenLimitX = 400.0f;        // Limite horizontal da tela

// Desenha as linhas de uma janela específica
void drawLines(const WindowData& windowData) {
    // Linha vermelha
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& point : windowData.redLinePoints) {
        glVertex2f(point.first, point.second);
    }
    glEnd();

    // Linha azul
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& point : windowData.blueLinePoints) {
        glVertex2f(point.first, point.second);
    }
    glEnd();
}

// Desloca os pontos de uma janela para a esquerda
void slideLinesLeft(WindowData& windowData) {
    for (auto& point : windowData.redLinePoints) {
        point.first -= pointMoveSpeed;
    }
    for (auto& point : windowData.blueLinePoints) {
        point.first -= pointMoveSpeed;
    }

    // Remove pontos fora da tela
    if (!windowData.redLinePoints.empty() && windowData.redLinePoints[0].first < 0) {
        windowData.redLinePoints.erase(windowData.redLinePoints.begin());
    }
    if (!windowData.blueLinePoints.empty() && windowData.blueLinePoints[0].first < 0) {
        windowData.blueLinePoints.erase(windowData.blueLinePoints.begin());
    }
}

// Adiciona um novo ponto aleatório a uma linha
void addPointToLine(std::vector<std::pair<float, float>>& linePoints, float newYValue, float& lastXValue) {
    float newXValue = lastXValue + pointMoveSpeed;
    linePoints.emplace_back(newXValue, newYValue);
    lastXValue = newXValue;
}

// Verifica se é necessário deslizar a tela
bool needsSlide(const WindowData& windowData) {
    return (!windowData.redLinePoints.empty() && windowData.lastRedPointX + pointMoveSpeed >= screenLimitX) ||
           (!windowData.blueLinePoints.empty() && windowData.lastBluePointX + pointMoveSpeed >= screenLimitX);
}

// Manipulador de teclado para interação global
void globalKeyboardHandler(unsigned char key, int x, int y) {
    if (key == '\r' || key == '\n') { // Enter pressionado
        // Verifica se é necessário deslizar usando apenas a primeira janela
        if (needsSlide(allWindowsData[0])) {
            for (auto& windowData : allWindowsData) {
                slideLinesLeft(windowData);
            }
        }

        // Adiciona novos pontos aleatórios para todas as janelas
        for (auto& windowData : allWindowsData) {
            float randomRedY = abs(static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f);
            float randomBlueY = abs(static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f);

            addPointToLine(windowData.redLinePoints, randomRedY, windowData.lastRedPointX);
            addPointToLine(windowData.blueLinePoints, randomBlueY, windowData.lastBluePointX);
        }

        // Atualiza todas as janelas
        for (int i = 0; i < numWindows; i++) {
            glutSetWindow(i + 1); // Define o foco na janela i
            glutPostRedisplay(); // Solicita atualização
        }
    }
}

// Função de exibição para uma janela específica
void displayWindow() {
    int windowIndex = glutGetWindow() - 1; // Identifica a janela atual
    glClear(GL_COLOR_BUFFER_BIT);

    drawLines(allWindowsData[windowIndex]);

    glutSwapBuffers();
}

// Inicializa e cria todas as janelas
void initializeWindows() {
    for (int i = 0; i < numWindows; i++) {
        std::string windowTitle = "Janela " + std::to_string(i + 1);
        glutCreateWindow(windowTitle.c_str());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, screenLimitX, 0.0, 1.0); // Define o sistema de coordenadas

        glutDisplayFunc(displayWindow);          // Callback para exibição
        glutKeyboardFunc(globalKeyboardHandler); // Callback para teclado
    }
}

// Função principal
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);

    initializeWindows();

    glutMainLoop();
    return 0;
}
