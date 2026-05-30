recursive subroutine quick_sort(arr, low, high)
    integer, intent(inout) :: arr(:)
    integer, intent(in) :: low, high
    integer :: pivot, temp, i, j, pi

    if (low < high) then
        pivot = arr(high)
        i = low - 1
        do j = low, high - 1
            if (arr(j) <= pivot) then
                i = i + 1
                temp = arr(i)
                arr(i) = arr(j)
                arr(j) = temp
            end if
        end do
        temp = arr(i + 1)
        arr(i + 1) = arr(high)
        arr(high) = temp
        pi = i + 1
        call quick_sort(arr, low, pi - 1)
        call quick_sort(arr, pi + 1, high)
    end if
end subroutine quick_sort

program main
end program main
