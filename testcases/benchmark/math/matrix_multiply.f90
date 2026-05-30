subroutine matrix_multiply(A, B, C, n)
    integer, intent(in) :: n
    integer, intent(in) :: A(n, n), B(n, n)
    integer, intent(out) :: C(n, n)
    integer :: i, j, k

    do i = 1, n
        do j = 1, n
            C(i, j) = 0
            do k = 1, n
                C(i, j) = C(i, j) + A(i, k) * B(k, j)
            end do
        end do
    end do
end subroutine matrix_multiply

program main
end program main
