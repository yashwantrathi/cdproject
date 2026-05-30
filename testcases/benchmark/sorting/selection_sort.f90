subroutine selection_sort(arr, n)
    integer, intent(inout) :: arr(:)
    integer, intent(in) :: n
    integer :: i, j, min_idx, temp

    do i = 1, n - 1
        min_idx = i
        do j = i + 1, n
            if (arr(j) < arr(min_idx)) then
                min_idx = j
            end if
        end do
        temp = arr(min_idx)
        arr(min_idx) = arr(i)
        arr(i) = temp
    end do
end subroutine selection_sort

program main
end program main
