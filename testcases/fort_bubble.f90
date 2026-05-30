subroutine bubbleSort(arr, n)
    implicit none
    integer, intent(inout) :: arr(n)
    integer, intent(in) :: n
    integer :: i, j, temp
    do i = 1, n-1
        do j = 1, n-i
            if (arr(j) > arr(j+1)) then
                temp = arr(j)
                arr(j) = arr(j+1)
                arr(j+1) = temp
            end if
        end do
    end do
end subroutine bubbleSort
