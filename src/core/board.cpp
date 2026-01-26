#include "board.hpp"
#include <imgui.h>

void DrawBoard()
{
    ImGui::Begin("Plateau de jeu");

    // taille de chaque case
    float  cell = 70.f;
    ImVec2 size{cell, cell};

    // double boucle pour les 64 cases (8 lignes et 8 colonnes)
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            // On donne un identifiant unique à chaque case (de 0 à 63)
            ImGui::PushID(y * 8 + x);

            // Calcul pour les couleurs de façon à faire un damier (somme paire -> une couleur, somme impaire -> une autre couleur)
            bool light = ((x + y) % 2 == 0);

            // définition des couleurs
            ImVec4 base = light
                              ? ImVec4{173 / 255.f, 232 / 255.f, 244 / 255.f, 1.f} // clair
                              : ImVec4{2 / 255.f, 62 / 255.f, 138 / 255.f, 1.f};   // sombre

            // on applique les couleurs aux boutons
            ImGui::PushStyleColor(ImGuiCol_Button, base);

            // hover
            ImVec4 hover = base;
            hover.x      = (hover.x + 0.08f > 1.f) ? 1.f : hover.x + 0.08f;
            hover.y      = (hover.y + 0.08f > 1.f) ? 1.f : hover.y + 0.08f;
            hover.z      = (hover.z + 0.08f > 1.f) ? 1.f : hover.z + 0.08f;

            ImVec4 click = base;
            click.x      = (click.x - 0.08f < 0.f) ? 0.f : click.x - 0.08f;
            click.y      = (click.y - 0.08f < 0.f) ? 0.f : click.y - 0.08f;
            click.z      = (click.z - 0.08f < 0.f) ? 0.f : click.z - 0.08f;

            // A MODIFIER POUR FAIRE LES HOVER ET CLICK
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, click);

            ImGui::Button(" ", size);

            ImGui::PopStyleColor(3);
            ImGui::PopID();

            if (x < 7)
                ImGui::SameLine();
        }
    }

    ImGui::End();
}
