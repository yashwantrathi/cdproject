module merge_sort_module
    implicit none
contains

    subroutine merge_arrays(arr, l, m, r)
        integer, intent(inout) :: arr(:)
        integer, intent(in) :: l, m, r
        integer :: i, j, k, n1, n2
        integer, allocatable :: L_arr(:), R_arr(:)
        
        n1 = m - l + 1
        n2 = r - m
        allocate(L_arr(n1), R_arr(n2))
        
        do i = 1, n1
            L_arr(i) = arr(l + i - 1)
        end do
        do j = 1, n2
            R_arr(j) = arr(m + j)
        end do
        
        i = 1; j = 1; k = l
        do while (i <= n1 .and. j <= n2)
            if (L_arr(i) <= R_arr(j)) then
                arr(k) = L_arr(i); i = i + 1
            else
                arr(k) = R_arr(j); j = j + 1
            end if
            k = k + 1
        end do
        
        do while (i <= n1)
            arr(k) = L_arr(i); i = i + 1; k = k + 1
        end do
        do while (j <= n2)
            arr(k) = R_arr(j); j = j + 1; k = k + 1
        end do
        
        deallocate(L_arr, R_arr)
    end subroutine merge_arrays

    recursive subroutine merge_sort(arr, l, r)
        integer, intent(inout) :: arr(:)
        integer, intent(in) :: l, r
        integer :: m
        if (l < r) then
            m = l + (r - l) / 2
            call merge_sort(arr, l, m)
            call merge_sort(arr, m + 1, r)
            call merge_arrays(arr, l, m, r)
        end if
    end subroutine merge_sort

end module merge_sort_module

program main
    use merge_sort_module
end program main

