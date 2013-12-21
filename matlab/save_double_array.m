function save_double_array(filename, matrix, data_type)

	fid = fopen(filename, 'wb');

	[rows, cols] = size(matrix);
	fwrite(fid, rows, 'double');

	for i=1:rows
		fwrite(fid, cols, 'double');
		fwrite(fid, matrix(i,:), data_type);
	end

	fclose(fid);
