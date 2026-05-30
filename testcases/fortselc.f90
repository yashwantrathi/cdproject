program selection_sort
    implicit none

    integer :: arr(5)
    integer :: i

    arr = (/ 64, 25, 12, 22, 11 /)

    print *, "Original array:"
    do i = 1, 5
        print *, arr(i)
    end do

    call selectionSort(arr, 5)

    print *, "Sorted array:"
    do i = 1, 5
        print *, arr(i)
    end do

contains

    subroutine selectionSort(arr, n)
        implicit none

        integer, intent(inout) :: arr(n)
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
    end subroutine selectionSort

end program selection_sort
