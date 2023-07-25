#include "imgui.h"

#include "Application.h"
#include "Sorts.h"

#include <future>
#include <algorithm>
#include <vector>
#include <cmath>

// debug
#include <iostream>
#include <sstream>

/*
* todos
*
* ability to cancel midway
* average over N trials, with different initializations for each trial
* better size slider finer granularity at higher regions
* run over multiple sizes, compile results together
* write to csv
* ImPlot visualizations 
* cleanup
*/

namespace Application
{
    enum SortStatus {
        SortStatusAwait = 0,
        SortStatusSorting = 1,
        SortStatusFinished = 2,
    };

    static SortStatus status_sort = SortStatusAwait;
    static bool should_init_sorted = false;
    static bool should_verify_sorted = true;

    static int log2_num_elements = 4; // num_elements = pow(_, 2)
    // inclusive
    static constexpr int lower_lim = 100000;
    static constexpr int upper_lim = -100000;

    static std::future<void> TestFuture;

    const bool default_checked = true;

    static std::vector<std::tuple<const char*, std::function<void(int*, int)>, bool>> todos{
        {"insertion sort", &insertion_sort, default_checked},
        {"heap sort", &heap_sort_recursive, default_checked},
        {"merge sort buffered", &merge_sort_buffered, default_checked},
        {"quick sort", &quick_sort, default_checked},
        {"radix sort", &radix_sort, default_checked},  
        {"std::sort", &std_sort, default_checked},
    };

    typedef std::tuple<const char*, bool, double> result;
    static std::vector<result> results;
    static ImGuiTableSortSpecs* current_sort_specs = nullptr;

    static float rtsf = 0.5; // responsivity test slider float

    void Init() {}

    void RenderUI()
    {
#ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
#else 
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("Sorts Test", (bool*) 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

        std::stringstream s;
        switch (status_sort)
        {
        case SortStatusAwait:
            ImGui::Checkbox("Initialize Array Already Sorted", &should_init_sorted);
            ImGui::Checkbox("Verify Sorted Correctly", &should_verify_sorted);
            ImGui::Dummy(ImVec2(0.0f, 20.0f));

            ImGui::Text("Sort Methods To Benchmark");
            for(auto& todo : todos)
            {
                const char* name = std::get<0>(todo);
                bool* should_test = &(std::get<2>(todo));     

                //std::cout << &todo  << " "  << &should_test << name << " " << "\n";

                ImGui::Checkbox(name, should_test);
            }
            ImGui::Dummy(ImVec2(0.0f, 20.0f));

            ImGui::Text("Number of Elements");
            s << std::to_string((long)pow(2, log2_num_elements));
            ImGui::SliderInt(" ", &log2_num_elements, 0, 28, s.str().c_str());
            ImGui::Dummy(ImVec2(0.0f, 20.0f));

            if (ImGui::Button("Start Tests"))
            {
                status_sort = SortStatusSorting;

                // hold onto the future in static variable
                // otherwise it will be destroyed at the end of this scope
                // its destructor waits until the task completes,
                // and since the destructor runs on the main thread,
                // this will block our main thread.
                // therefore dont allow the future to be destroyed!

                TestFuture = std::async(std::launch::async, RunTests, pow(2, log2_num_elements));
            }

            break;
        case SortStatusSorting:
        {
            ImGui::Text("Running Tests...");
            ImGui::SliderFloat("Responsivity Test Slider", &rtsf, 0, 1);
            break;
        }
        case SortStatusFinished:
        {
            std::stringstream s_stream;
            s_stream << "SORTING PARAMETERS\nAlready Sorted:      " << (should_init_sorted ? "TRUE" : "FALSE") << "\nNumber of Elements:  " << std::to_string((long)pow(2, log2_num_elements)) << "\n\n";
            ImGui::Text(s_stream.str().c_str());

            static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                    // 
                ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_NoBordersInBody;


            static const int MAX_ROWS = 8; // excluding header row
            ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * (1 + std::min((int) (results.size()), MAX_ROWS)));
            ImGui::BeginTable("Results", 3, flags, outer_size);
            
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

            ImGui::TableSetupColumn("Sort Method", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Passed", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Time (s)", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            for (auto r : results)
            {
                //<const char*, bool, double>
                const char* name = std::get<0>(r);
                bool passed = std::get<1>(r);
                double dur = std::get<2>(r);

                //s_stream << std::setprecision(10) << name << (passed ? " passed in " : " failed in ") << dur << "s\n";

                ImGui::TableNextRow();

                ImU32 cell_bg_color = ImGui::GetColorU32(
                    should_verify_sorted ?
                    (passed ? ImVec4(0.0f, 1.0f, 0.0f, 0.5f) // green
                        : ImVec4(1.0f, 0.0f, 0.0f, 0.5f)) // red
                    : ImVec4(0.1f, 0.1f, 0.1f, 0.5f) // black
                );

                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);

                ImGui::TableSetColumnIndex(0);
                ImGui::Text(name);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text(should_verify_sorted ? (passed ? "Y" : "N") : "?");

                ImGui::TableSetColumnIndex(2);
                ImGui::Text(std::to_string(dur).c_str());
                    
                }
            }
            
            // Sort our data if sort specs have been changed!
            if (ImGuiTableSortSpecs* table_sorts_specs = ImGui::TableGetSortSpecs())
                if (table_sorts_specs->SpecsDirty)
                {
                    current_sort_specs = table_sorts_specs; // Store in variable accessible by the sort function.

                    if (results.size() > 1) {
                        std::sort(results.begin(), results.end(), [](const result& lhs, const result& rhs)
                            {
                                bool dir = current_sort_specs->Specs->SortDirection == 1;
                                switch (current_sort_specs->Specs->ColumnIndex)
                                {
                                case 0:
                                    return (std::get<0>(lhs) < std::get<0>(rhs)) == dir;
                                case 1:
                                    return (std::get<1>(lhs) < std::get<1>(rhs)) == dir;
                                case 2:
                                    return (std::get<2>(lhs) < std::get<2>(rhs)) == dir;
                                default:
                                    return false;
                                }
                            }
                        );
                    }

                    current_sort_specs = nullptr;
                    table_sorts_specs->SpecsDirty = false;
                }

            

            ImGui::EndTable();


            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            if (ImGui::Button("Start Again"))
            {
                status_sort = SortStatusAwait;
            }
            break;
        }
        ImGui::End();
        ImGui::PopStyleVar(1);

        ImGui::ShowDemoWindow();
    }


    void RunTests(int length)
    {
        srand(time(0));

        // initialize source array
        int* source_array = new int[length];
        if (should_init_sorted)
        {
            for (int i = 0; i < length; i++)
            {
                source_array[i] = i;
            }
        }
        else
        {
            for (int i = 0; i < length; i++)
            {
                source_array[i] = rand() % (upper_lim - lower_lim + 1) - lower_lim;
            }
        }

        // allocate memory for array to be sorted by each test (will be reinitialized to source_array before each test)
        int* array = new int[length];

        results.clear();

        for (auto& todo : todos)
        {
            bool should_test = std::get<2>(todo);
            if (should_test)
            {
                const char* name = std::get<0>(todo);
                std::function<void(int*, int)>& func = std::get<1>(todo);

                auto [success, duration] = test(source_array, array, length, should_verify_sorted, name, func);

                results.push_back({ name, success, duration });
            }
        }

        // free memory
        delete[] source_array;
        delete[] array;

        status_sort = SortStatusFinished;
    }

}
