function a = load_double_array (filename, data_type)

	fid = fopen(filename, 'rb');
	rows = fread(fid, 1, 'double');
	cols = fread(fid, 1, 'double');

	a = zeros(rows, cols, data_type);
	a(1,:) = fread(fid, cols, data_type);
	for i = 2:rows
		fread(fid, 1, 'double');
		a(i,:) = fread(fid, cols, data_type);
	end

	fclose(fid);

end