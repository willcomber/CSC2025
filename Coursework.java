package coursework;

import static spark.Spark.*;

import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.spark.SparkConf;
import org.apache.spark.api.java.JavaSparkContext;
import org.apache.spark.sql.Dataset;
import org.apache.spark.sql.Encoders;
import org.apache.spark.sql.Row;
import org.apache.spark.sql.SparkSession;
import org.apache.spark.api.java.function.*;

import classTutorial.Messages;

/**
 * 
 * @author 170168146 - Will Comber
 *
 */
public class Coursework {
	private static String PATH = Messages.getString("LearningSparkIntroExamples.0"); //$NON-NLS-1$

	static SparkConf conf = new SparkConf().setMaster("local").setAppName("My App"); //$NON-NLS-1$ //$NON-NLS-2$
	static JavaSparkContext sc = new JavaSparkContext(conf);
	static SparkSession spark = SparkSession.builder().appName("Java Spark SQL basic example") //$NON-NLS-1$
			.config("spark.some.config.option", "some-value").getOrCreate(); //$NON-NLS-1$ //$NON-NLS-2$

	public static void main(String[] args) {

		LogManager.getLogger("org").setLevel(Level.ERROR); //$NON-NLS-1$

		// Step 1 read in both csv files all options
		System.out.println("\n****** Step 1 ******");
		Dataset<Row> ratings = spark.read().option("inferSchema", true).option("header", true).option("multLine", true)
				.option("mode", "DROPMALFORMED").csv(PATH + "/ratings.csv");
		Dataset<Row> movies = spark.read().option("inferSchema", true).option("header", true).option("multLine", true)
				.option("mode", "DROPMALFORMED").csv(PATH + "/movies.csv");

		System.out.println("Ratings schema:");
		ratings.printSchema(); // print schema of both datasets
		System.out.println("Movies schema:");
		movies.printSchema();

		// Step 2 flatmap discovered via
		// https://stackoverflow.com/questions/31008169/spark-rdd-mapping-one-row-of-data-into-multiple-rows
		System.out.println("\n****** Step 2 ******");

		// get just Id and genre, use flat map to convert each row into multiple
		// but type can no longer be row so now a string
		Dataset<String> idGenre = movies.select("movieId", "genres").flatMap(
				(FlatMapFunction<Row, String>) row -> genreSplitterList(row.getInt(0), row.getString(1)).iterator(),
				Encoders.STRING());

		idGenre.show(5);

		// save to CSV user print writer
		try {
			List<String> rows = idGenre.collectAsList();
			PrintWriter out = new PrintWriter(new File(PATH + "/movieGenres.csv"));
			out.println("movieId,genre"); // save header
			for (String row : rows) {
				out.println(row); // save each row (already formatted)
			}
			out.close(); // close print writer
			System.out.println("Succesfully written to file");
		} catch (Exception e) {
			System.out.println("Error writing file or collecting as list: " + e);
			// added in as collect as list doesn't work on own computer
		}

		// Step 3
		System.out.println("\n****** Step 3 ******");
		// read in file created in step 2
		Dataset<Row> movieGenres = spark.read().option("inferSchema", true).option("header", true)
				.option("multLine", true).option("mode", "DROPMALFORMED").csv(PATH + "/movieGenres.csv");

		movieGenres.createOrReplaceTempView("IdGenre"); // allow sql to use
														// dataset

		movieGenres.printSchema();
		movieGenres.orderBy(movieGenres.col("movieId").desc()).show(50);
		// show 50 ordered by highest movieId first

		// Step 4
		System.out.println("\n****** Step 4 ******");
		Dataset<Row> genrePopularity = spark
				.sql("SELECT genre, count(genre) AS movieCount FROM IdGenre GROUP BY genre ORDER BY movieCount DESC");
		// sql used as query is simpler to write
		genrePopularity.show(10); // check data is right

		// Step 5
		System.out.println("\n****** Step 5 ******");
		List<Row> genres = genrePopularity.select("genre").takeAsList(10);
		// take top 10 genres (already ordered)

		Dataset<Row> joinedUMG = movieGenres.join(ratings, movieGenres.col("movieId").equalTo(ratings.col("movieId")))
				.select(ratings.col("userId"), ratings.col("movieId"), movieGenres.col("genre"));
		joinedUMG.createOrReplaceTempView("Joined"); // allow to be used in sql
		// creating joined table once to save on computation time

		try {
			Row user;
			// output header for rows
			System.out.println("<genre, top_user>");

			for (Row gRow : genres) {
				// for each genre reduce results to movies of type genre then
				// group on userId
				// and count number of appearances
				user = spark.sql("SELECT userId, count(userId) AS no_reviewed FROM Joined WHERE genre = '"
						+ gRow.getString(0) + "' GROUP BY userId ORDER BY no_reviewed").takeAsList(1).get(0);
				// take 1 in a list and get first in list
				System.out.println("<" + gRow.getString(0) + ", " + user.getInt(0) + ">");
			}
		} catch (Exception e) {
			System.out.println("Computation likely timed out: " + e);
			// added in as this happens on own computer
		}

		// Step 6
		System.out.println("\n****** Step 6 ******");
		// allow ratings and movie genres to be used in sql queries
		ratings.createOrReplaceTempView("Ratings");
		movieGenres.createOrReplaceTempView("MovieGenres");

		Dataset<Row> ratingsCount = spark.sql(
				"SELECT userId, count(userId) AS ratingsCount FROM Ratings GROUP BY userId ORDER BY ratingsCount DESC");
		ratingsCount.show(5); // get and then show top 5 reviewers to check has worked

		ratingsCount.createOrReplaceTempView("AllRatingsCount"); // allow to be used in sql

		List<Row> topReviewers = ratingsCount.takeAsList(10); // take top 10
																// reviewers
		try {
			Row user;
			// output header for rows
			System.out.println("<user, ratingsCount, top_genre>");
			for (Row topReviewer : topReviewers) {
				user = spark
						.sql("SELECT genre, count(genre) AS genreCount FROM MovieGenres, Ratings WHERE userId = "
								+ topReviewer.getInt(0)
								+ " AND MovieGenres.movieId = Ratings.movieId GROUP BY genre ORDER BY genreCount DESC")
						.takeAsList(1).get(0);
				// select count of genre and order by this, then take as list
				// and select 1 to
				// get most popular genre
				System.out.println(
						"<" + topReviewer.getInt(0) + ", " + topReviewer.getInt(1) + ", " + user.getString(0) + ">");
			}
		} catch (Exception e) {
			System.out.println("Computation likely timed out:" + e);
			// added in as this happens on own computer
		}

		// Step 7
		System.out.println("\n****** Step 7 ******");
		// sql has commands to calculate average and variance so these are used
		// top 10 avg ratings shown likely to have very low variance (or not a
		// number) as will be one review of 5 (probably)
		spark.sql("SELECT movieId, AVG(rating) AS average_rating, variance(rating) AS variance_rating FROM Ratings "
				+ "GROUP BY movieId ORDER BY average_rating DESC").show(10);

	}

	/**
	 * return list of row broken down into one for each genre
	 * 
	 * @param id
	 * @param genres
	 * @return
	 */
	private static List<String> genreSplitterList(int id, String genres) {
		List<String> result = new ArrayList<String>();
		String[] split = genres.split("\\|"); // | is a reserved char in a regex
		for (String s : split) {
			result.add(id + "," + s); // for each split add new 'row' to list in
										// csv format
		}
		return result;

	}
}