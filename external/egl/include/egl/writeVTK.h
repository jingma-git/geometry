#pragma once

namespace egl
{
    template <typename OS, typename FLOAT, typename INT>
    void tri2vtk(
        OS &os,
        const FLOAT *node, size_t node_num,
        const INT *tri, size_t tri_num)
    {
        os << "# vtk DataFile Version 2.0\nTRI\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";

        os << "POINTS " << node_num << " float\n";
        for (size_t i = 0; i < node_num; ++i)
            os << node[i * 3 + 0] << " " << node[i * 3 + 1] << " " << node[i * 3 + 2] << "\n";

        os << "CELLS " << tri_num << " " << tri_num * 4 << "\n";
        for (size_t i = 0; i < tri_num; ++i)
            os << 3 << "  " << tri[i * 3 + 0] << " " << tri[i * 3 + 1] << " " << tri[i * 3 + 2] << "\n";
        os << "CELL_TYPES " << tri_num << "\n";
        for (size_t i = 0; i < tri_num; ++i)
            os << 5 << "\n";
    }

    template <typename OS, typename Iterator, typename INT>
    void vtk_data(OS &os, Iterator first, INT size, const char *value_name, const char *table_name = "my_table")
    {
        os << "SCALARS " << value_name << " float\nLOOKUP_TABLE " << table_name << "\n";
        for (size_t i = 0; i < size; ++i, ++first)
            os << *first << "\n";
    }
}