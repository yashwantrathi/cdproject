recursive subroutine mergeSort(arr, left, right)
    integer, intent(inout) :: arr(*)
    integer, intent(in) :: left, right
    integer :: mid
    if (left < right) then
        mid = (left + right) / 2
        call mergeSort(arr, left, mid)
        call mergeSort(arr, mid + 1, right)
    end if
end subroutine mergeSort
